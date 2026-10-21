//purpose of the code: To make a system where there are 3 traffic signals which activate on the basis of a pre define sequence but whena a pedestrian 
//press a button, the current traffic state is executes the current state nnd then stops the teaffic. there is a cool off for the pedestrian button.
// of 10s



/* order of things:



pin setups:
PB0 -> Button
PB1 -> green
PB2 -> yellow 
PB3 -> Blue(which will be trwated as red in this code), sice i domnt have a red LED.
PB4 -> Pedestrian LED. 


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



volatile uint8_t b_press = 0;
volatile uint32_t count_tick = 0;

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
  static uint8_t count = 0;
  static uint8_t last_state = 1;
  static uint8_t last_stable_state = 1;
  int raw = (pinb & (1<<0)) ? 1 : 0;
  if(raw != last_state){
    count = 0;
    last_state = raw;

  }
  else if(last_state == raw){
    count++;
    if(count == 10 && (last_stable_state != raw)){
      if(last_stable_state == 1 && raw == 0){
        b_press = 1;
      }
      last_stable_state = raw;
    }
  }
  count_tick++;
}


uint32_t millis_s(){
  return count_tick;
}

void setup(){
  timer_setup();
  ddrb &= ~(1<<0);
  portb |= (1<<0);
  ddrb |= (1<<1);
  ddrb |= (1<<2);
  ddrb |= (1<<3);
  ddrb |= (1<<4);

}

// PB0 -> Button
// PB1 -> green
// PB2 -> yellow 
// PB3 -> Blue(which will be trwated as red in this code), sice i domnt have a red LED.
// PB4 -> Pedestrian LED. 

//control is a flag which disables the contorl other leds have when one led is glowing. 

static uint8_t state = 1; // the sequence will always begin with the green LED turning on first.
uint32_t last = 0;



void loop(){
  uint32_t now = millis_s();
  //set state by checking the flags and the last time.
  if(state == 1){
    if(now - last == 7000){
      last = now;
      if(b_press){
      state = 4;
      b_press = 0;
      }
      else {
        state = 2;
      }
    }
  }
  else if(state == 2){
    if(( (now - last) == 2000) ){
      last = now;
      if(b_press){
        b_press  = 0;
        state = 4;
      }
    
      else{
        state = 3;
      }
    }
  }
  else if( state == 3 ){
    if( (now - last == 8000) ){
    last = now;
    if(b_press){
      b_press = 0;
      state = 4;
    }
    else{
      state = 1;
    }
  }
  }

  else if(state == 4){
    if(now - last == 1000){
    state = 1;
    last = now;
    }
  }
// where i left off: add the state description for state 4 and then add some more stuff like connecting the portb4 led for ped led
  
  switch(state){
    case 1:
    portb |= (1<<1);
    portb &= ~(1<<2);
    portb &= ~(1<<3);
    portb &= ~(1<<4);
    break;

    case 2:
    portb &= ~(1<<1);
    portb |= (1<<2);
    portb &= ~(1<<3);
    portb &= ~(1<<4);
    break;

    case 3:
    portb &= ~(1<<1);
    portb &= ~(1<<2);
    portb |= (1<<3);
    portb &= ~(1<<4);
    break;
    
    case 4:
    portb &= ~(1<<1);
    portb &= ~(1<<2);
    portb &= ~(1<<3);
    portb |= (1<<4);
    break;
  
  }
  //update states and check the button. 
}