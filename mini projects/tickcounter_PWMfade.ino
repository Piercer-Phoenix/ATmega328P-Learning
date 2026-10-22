// Purpose of the code: a pwm is continuously faded and reset(fast PWM), and manwhile, for every 2 seconds the led turns off and another LED turns on
// this is essentially a tick counter program, and also a PWM exercise to programaticcaly, using a for loop, vary a pwm signal. on its own. 


/* Order of things:
One time run code: 
1) Timer0 setup (which will be used for PWMs on both the LEDs)
2) Timer2 setup (which will be used for delay functions)


Multi run code: 
1) PWM function using timer0, 
2) ISR for counting ticks, and measuring the time non blocking.
3) Main loop which looks after the pwm signals and the timing. 


connections: 
pd5 -> led1
pb6 -> led2
*/


#define tccr0a *((volatile byte*) 0x44)
#define tccr0b *((volatile byte*) 0x45)
#define timsk0 *((volatile byte*) 0x6E)
#define ocr0a *((volatile byte*) 0x47)
#define tcnt0 *((volatile byte*) 0x46)
#define ocr0b *((volatile byte*) 0x48)
#define ddrd *((volatile byte*) 0x2A)
#define portd *((volatile byte*) 0x2B)
#define prr *((volatile byte*) 0x64)


#define tcnt2 *((volatile byte*) 0xB2)
#define tccr2a *((volatile byte*) 0xB0)
#define tccr2b *((volatile byte*) 0xB1)
#define ocr2a *((volatile byte*) 0xB3)
#define timsk2 *((volatile byte*) 0x70)





void timer2_setup(){
  //step1: set the mode to ctc mode
  tccr2b &= ~(1<<3);
  tccr2a |= (1<<1);
  tccr2a &= ~(1<<0);
  //step2: set the prescaler = 128.

  tccr2b |= (1<<2);
  tccr2b &= ~(1<<1);
  tccr2b |= (1<<0);

  //step3:enabling interrupts for ocf2a
  timsk2 |= (1<<1);
  //step 4: init ocr2a to 125 and set tcnt2 to 0.

  tcnt2 = 0;
  ocr2a = 125;
}


volatile uint32_t count_tick = 0;

ISR(TIMER2_COMPA_vect){
  count_tick++;
}

uint32_t millis_s(){
  return count_tick;
}


void timer0_setup(){

// the com bits will be set in the main loop.
  //step 1: clear the PRTIM0 bit to enable the timer
  prr &= ~(1<<5);

  //step 2: Enabling the fast PWM mode, specifically mode 3,  of operation by setting the WGM bits in the tccr0x registers
  tccr0a |= (1<<0);  
  tccr0a |= (1<<1);  
  tccr0b &= ~(1<<3);

  //step 3: setting the cs01:00 bits and clear cs02 bit, using the tccr0b register:

  tccr0b &= ~(1<<0);
  tccr0b |= (1<<1);
  tccr0b &= ~(1<<2);


  //step 4: init the tcnt0 to 0 
  tcnt0 = 0;
}

void timer0_pwm(uint8_t input){
  ocr0a = input;
  ocr0b = input;
}




void setup(){
  timer2_setup();
  timer0_setup();
  ddrd |= (1<<5);
  ddrd |= (1<<6);
}
uint8_t count = 0;
void loop(){
  static uint32_t last = 0;
  static uint8_t control = 0;
  static uint32_t strobe_last = 0;
  uint32_t strobe_now = millis_s();
  uint32_t now = millis_s();
  if(now - last >= 2000){
    //change the pins, 
    control ^= 1;
    last = now;
    count = 0;
  }
  if(strobe_now - strobe_last >= 60){
    
    strobe_last = strobe_now;
    count += 10;
  }

  if(control == 1){
    tccr0a |= (1<<7);
    tccr0a &= ~(1<<6);
    tccr0a &= ~(1<<5);
    tccr0a &= ~(1<<4);
    timer0_pwm(count);
  }
  else{
    tccr0a &= ~(1<<7);
    tccr0a &= ~(1<<6);
    tccr0a |= (1<<5);
    tccr0a &= ~(1<<4);
    timer0_pwm(count);
  }                                          
}