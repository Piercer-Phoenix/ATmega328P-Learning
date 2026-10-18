// Code Purpose: The code is supposed to read 2 buttons actively and one button controls the functionality of the other button which can toggle 
//the led. B1 allows B2 to control the LED. In a off or on state. Here we will use timer, and GPIO basics.


/* Order of things:
flags:
1) B1 state
2) B2 state 

NOTE: B1 -> Master button, PORTB3
      B2 -> Slave button, PORTB4
      LED -> PORTB5

Single time execution parts of the program:
1) Timer setup for firing the ISR for compare match , every 1ms.
2) Setting up inputs and outputs. 

Multiple time execution parts of the program:
1) ISR - state maintanance. 
2) Main loop - state flags consumer and toggles the control flag, and the other if statement checks the control flag and the button press
and toggles the LED. 

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



volatile uint8_t b1 = 0;
volatile uint8_t b2 = 0;
volatile uint8_t b1_press = 0;
volatile uint8_t b2_press = 0;
volatile uint8_t control = 0;


void timer_setup(){
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



void setup() {
  Serial.begin(9600);
  timer_setup();
  //setting the data ddirection for the buttons and the led
  ddrb &= ~(1<<4);
  ddrb &= ~(1<<3);
  portb |= (1<<3); //
  portb |= (1<<4);
  ddrb |= (1<<5);
}

void loop() {
  if(b1_press){
    control ^= 1;
    b1_press = 0; 
    
  }
  if(control){
    if(b2_press){
      portb ^= (1<<5);
      b2_press = 0;
      Serial.println("B2 Press");
    }
  }


}
