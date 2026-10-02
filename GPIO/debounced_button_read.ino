#define ddrb *((volatile byte*) 0x24)
#define portb *((volatile byte*) 0x25) 
#define pinb *((volatile byte*) 0x23)
#define tccr0a *((volatile byte*) 0x44)
#define tccr0b *((volatile byte*) 0x45)
#define tcnt0 *((volatile byte*) 0x46)
#define ocr0a *((volatile  byte*) 0x47)
#define tifr0 *((volatile byte* ) 0x35)
#define timsk0 *((volatile byte*) 0x6E)









/*
*) Order of things:
1) Select the CTC mode, 
2) Set the value of OCR0A as 250, 
3) Select prescaler as 64, (for one ms window everytime an interrupt is raised)
4) enable interrupt for timer0's COMPA
5) set PB4 as ooutput and pb0 as input and enable the internal pull up for PB0 by writing logical one to portb0. 
6) define the ISR, which reads the state of the Pxn pin every millisecond(defined by timer0), and sets flags, a more detailed 
explaination is in the document
7) in the main loop, use the flags set by the ISR. 
*/

volatile uint8_t button_pressed = 0;


ISR(TIMER0_COMPA_vect){
  
  static uint8_t count = 0;
  static uint8_t last_state = 1;
  static uint8_t last_stable_state = 1;

  uint8_t curr_state = pinb & (1<<0) ? 1 : 0;

  if(curr_state != last_state){
    count = 0;
    last_state = curr_state;
    last_stable_state = 1;
  } 

  else {
    count++;
    if(count == 10 && (last_stable_state != curr_state)){
      if(last_stable_state == 1 && curr_state == 0)
      {
      button_pressed = 1;
      last_stable_state = curr_state;
      }
    }
  }

}




void setup() {
  //since we are using timer0, we need to write binary 010 to wgm02:00 bits. 
  //step1: configure timer to ctc
  tccr0b &= ~(1<<3); //wgm02 bit
  tccr0a |= (1<<1); //wgm01 bit
  tccr0a &= ~(1<<0); //wgm00 bit


  //step 3: configuring the prescaler, we have to write 011(in binrary form) to CS02:00 bits for setting the prescaler as 64. 
  tccr0b &= ~(1<<2);
  tccr0b |= (1<<1);
  tccr0b |= (1<<0);
  
  //step 4: enabling the interrupt for ocf0a by setting the OCIE0A bit in timsk0
  timsk0 |= (1<<1);

  //step5: 

  ddrb |= (1<<4); 
  ddrb &= ~(1<<0);
  portb |= (1<<0);

  //step 2: setting ocr0a to the designated value, and rset the timer jjust for the sake of it,
  ocr0a = 250; 
  tcnt0 = 0; 

}

void loop() {
  //step 7: 
  if(button_pressed){
    //do what is supposed to be done, which in this case will be toggling led, which is on pb4
    portb ^= (1<<4);    
    button_pressed = 0;         
  }
}