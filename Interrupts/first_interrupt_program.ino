//defining the macros for addressing registers. 
#define tcnt2 *((volatile byte*) 0xB2)
#define tccr2a *((volatile byte*) 0xB0)
#define tccr2b *((volatile byte*) 0xB1)
#define ocr2a *((volatile byte*) 0xB3)
#define ocr2b *((volatile byte*) 0xB4)
#define tifr2 *((volatile byte*) 0x37)
#define ddrb  *((volatile byte*) 0x24)
#define portb *((volatile byte*) 0x25)
#define prr *((volatile byte*) 0x64)
#define timsk2 *((volatile byte*) 0x70)



//This is a simple program to blink an LED, but using timer interrupts, the custom delay mechanism is going to be interrupt driven , this code will
// using the timer2 module for the delay mechanism. 

/* Flow of events in this code:
1) configure the timer2 module into CTC mode, 
2) Select the prescaler for the timer2 module, I will be using the prescaler of 256, since this clock signal setup generates a ocf0a flag for 4 microseconds precisely 
3) enable the timer2 module using the PRTIM2 bit of the PRR register.
4) Enable interrupts for the ocf2a bit. 
5) set tcnt2 to 0 and ocr2a to 250
6) Write an ISR for what to do when ocf2a generates an interrupt. 


*/





uint32_t last_time = 0;
volatile uint32_t current_time = 0;


void setup() {
  ddrb |= (1<<0);
  
  // step1: configuring timer 2 to ctc mode
  tccr2b &= ~(1<<3);
  tccr2a |= (1<<1);
  tccr2a &= ~(1<<0);

  //step2: selecting the prescaler 
  tccr2b &= ~(1<<0);
  tccr2b |= (1<<1);
  tccr2b |= (1<<2);

  //step3: enabling the timer2 module 
  prr &= ~(1<<6);

  //step4: enabling timer interrupt for ocf2a
  timsk2 |= (1<<1);  


  //step 5: 
  tcnt2 = 0;
  ocr2a = 250;
}

void loop() {
  cli();
  if( (current_time - last_time) == 250){
  portb ^= (1 << 0);
  last_time =current_time;
  }
  sei();
}


ISR(TIMER2_COMPA_vect){
  current_time++; // increment counter
}