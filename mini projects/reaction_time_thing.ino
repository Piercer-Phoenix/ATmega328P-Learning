//purpose of the code: there is a led and 2 button, and the led will flash, and you have to press a button to start the game, it is called S_button
//and once S_button is pressed, after a delay, The led will flash, and you will have to press the response button( R_button) as fast as possible, and the 
//time will be recorded. 


/*
Order of things: 

press S_button, blink the LED, wait X ms, LED lights up for 300ms,and then, the LED turns off and u have to press R_button, 
and the result of the time taken will be displayed and sent to the PC and the S_button's press flag will have been used up, 
sothe system will wait for S_button press again. 
Single time code:
1) Setup timer0 - state machine maintanance 
2) setup timer2
3) disable timer2
2) Setup the led and the buttons 

Multiple run code:
1) The ISR statemachine 
2) the timer2 module which counts the reaction time
3) the main loop which disables or enables the timer2 module based on the button 
4) The ISR for counting time

flags: 
-> The b1 and b2 press flags, which are maintained by the statemachine ISR.
-> The b1 and b2 flags will be consumed by the main loop and enable the game, by waiting for a x amount of time(delay), where the LED will flash for 
300 ms and the moment it turns off, you've gotta press the R_button asap. 
-> Based on the press of the b1, the timer2(counter) is to be reset. 

b1 -> S_button
b2 -> R_button
*/





#define tccr0a *((volatile byte*) 0x44)
#define tccr0b *((volatile byte*) 0x45)
#define timsk0 *((volatile byte*) 0x6E)
#define prr *((volatile byte*) 0x64)
#define ocr0a *((volatile byte*) 0x47)
#define tcnt0 *((volatile byte*) 0x46)
#define tifr0 *((volatile byte*) 0x35)
#define portb *((volatile byte*) 0x25)
#define pinb *((volatile byte*) 0x23)
#define ddrb *((volatile byte*) 0x24)
#define timsk0 *((volatile byte*) 0x6E)


#define tcnt2 *((volatile byte*) 0xB2)
#define tccr2a *((volatile byte*) 0xB0)
#define tccr2b *((volatile byte*) 0xB1)
#define ocr2a *((volatile byte*) 0xB3)
#define ocr2b *((volatile byte*) 0xB4)
#define tifr2 *((volatile byte*) 0x37)
#define timsk2 *((volatile byte*) 0x70)


volatile uint8_t b1_press = 0;
volatile uint8_t b2_press = 0;
volatile uint32_t count = 0;




void timer0_setup(){
  //step1: disabling timer
  prr &= ~(1<<5);

  //configuring timer0 to ctc mode , setting WGM bits to 0-1-0
  tccr0b &= ~(1<<3);
  tccr0a |= (1<<1);
  tccr0a &= ~(1<<0);


  //setting prescaler: using cs2:0 bits to 64.

  tccr0b &= ~(1<<2);
  tccr0b |= (1<<1);
  tccr0b |= (1<<0);

  //enabling interrupt for ocf0a 

  timsk0 |= (1<<1); 

  //reset tcnt0 and initializing ocr0a to 250

  tcnt0 = 0;
  ocr0a = 250;
}

// statemachine and debouncer ISR
ISR(TIMER0_COMPA_vect){
  static int b1_last_state = 1;
  static int b1_last_stable_state = 1;
  static int b2_last_state = 1;
  static int b2_last_stable_state = 1;
  int b1_raw = pinb & (1 << 3) ? 1 : 0; // will return 1 always, if return 0, then contact.
  int b2_raw = pinb & (1 << 4) ? 1 : 0; // will return 1 always, if return 0, then contact. 
  static int count_b1 = 0;
  static int count_b2 = 0;

  if(b1_raw != b1_last_state){
    //change occured
    count_b1 = 0;
    b1_last_state = b1_raw;
  }
  else if(b1_raw == b1_last_state){
    count_b1++;
    if(count_b1==10 && (b1_last_stable_state != b1_raw) ){
      if(b1_raw == 0 && b1_last_stable_state == 1){ // condition for press, because we can only count press, not release. 
        b1_press = 1; //set press event flag.
      }b1_last_stable_state = b1_raw;
    }
  }


  if(b2_raw != b2_last_state){
    //change occured
    count_b2 = 0;
    b2_last_state = b2_raw;
  }
  else if(b2_raw == b2_last_state){
    count_b2++;
    if(count_b2==10 && (b2_last_stable_state != b2_raw)) //this condition signifies if a press or release happened )
    {
      //since last_stable_state has to change, for the flag. 
      if(b2_raw == 0 && b2_last_stable_state == 1){ // condition for press, because we can only count press, not release.
        b2_press = 1; //set press event flag.
      }
      b2_last_stable_state = b2_raw;
    }
  }
}


void timer2_setup(){
  //step1: selecting mode
  tccr2a &= ~(1<<0);
  tccr2a |= (1<<1);
  tccr2b &= ~(1<<3);
  // step 1 done.



  //step 2:
  tcnt2 = 0;
  ocr2a = 250;

  //init the timer by setting the prescalers. 256, and ocr will be 250
  tccr2b &= ~(1<<0);
  tccr2b |= (1<<1);
  tccr2b |= (1<<2);

  //step 3: 
  prr &= ~(1<<6);
  
}


void sleep_timer2(int n){ // n is the amount of miliseconds, 
  uint16_t stop = n / 4;
  uint16_t cnt = 0;
  uint8_t status;
  while(cnt < stop){
    status  = tifr2 & (1<<1) ? 1 : 0;
    if(status){
      cnt++;
      tifr2 |= (1<<1); // clear flag
    }
  }
  
}

ISR(TIMER2_COMPA_vect){
  count++;

}

void start_counter(){
  count = 0;
  timsk2 |= (1<<1);
}

int stop_counter(){
  int sample = count;
  timsk2 &= ~(1<<1);
  return sample;
}



void setup() {
  timer0_setup();
  timer2_setup();
  ddrb &= ~(1<<4);
  ddrb &= ~(1<<3);
  portb |= (1<<3); 
  portb |= (1<<4);
  ddrb |= (1<<5);
  Serial.begin(9600);
}

void loop() {
  if(b1_press){
    b1_press =0;
    Serial.println("B1_pressed");
    //S_button pressed, so, initiate the sequence, 
    portb |= (1<<5);
    sleep_timer2(300);
    portb &= ~(1<<5);
    sleep_timer2(3000);
    portb |= (1<<5); 
    sleep_timer2(300);
    portb &= ~(1<<5);
    start_counter();
    while(!b2_press){
      Serial.println("PRess the button lil bro");
    }
    uint32_t time  = stop_counter();
    Serial.print( (time * 4) );
    Serial.println(" milliSeconds");
    sleep_timer2(5000);
  }
}
