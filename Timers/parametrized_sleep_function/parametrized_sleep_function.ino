#define tcnt2 *((volatile byte*) 0xB2)
#define tccr2a *((volatile byte*) 0xB0)
#define tccr2b *((volatile byte*) 0xB1)
#define ocr2a *((volatile byte*) 0xB3)
#define ocr2b *((volatile byte*) 0xB4)
#define tifr2 *((volatile byte*) 0x37)
#define ddrb  *((volatile byte*) 0x24)
#define portb *((volatile byte*) 0x25)
#define prr *((volatile byte*) 0x64)


//for this experiment, ill be using the timer2 module, and its registers.
// one extra thing is to just enable the timer itself using the PRTIM2 bit. 
// this code is an increment in complexity to the previous 1s hardcoded slee function. 


/*
The order of thins in this code:
1) choosing the mode of the timer2, that is, configuring it to ctc.
2) init the tcnt2 module to 0 and ocr2a must be set to 250(as si the case for the previous example.)
3) write th e PRTIM2 bit to a logical 0..
4) write the CS bits to 1-1-1. to enable the timer and set the prescaler configuration. 
5) calculate the number of ofc2a's overflows limit , based on the parameter value of the function,and the calculation of the number of times the flag
is to be read.
6) set CS bits in tccr2b 0-2 bits. to 0-0-0, basically clear the CS bits for turning the timer off. (not required, really, as we are initializing the 
tcnt2 register everytime we call it)
*/







//coding section commencement:
void sleepself_extended(int n){
  //step1: selecting mode
  tccr2a &= ~(1<<0);
  tccr2a |= (1<<1);
  tccr2b &= ~(1<<3);
  // step 1 done.



  //step 2:
  tcnt2 = 0;
  ocr2a = 250;

  //init the timer by setting the prescalers. 
  tccr2b |= (1<<0);
  tccr2b |= (1<<1);
  tccr2b |= (1<<2);

  
  int overflows = n / 16;
  int status;
  //step 3: enable the timer2 module by writing PRTIM2(in PRR register) bit to 0. 
  prr &= ~(1<<6);
  
  //loop
  while(overflows > 0){

    status = tifr2 & (1<<1);
    if(status){
      overflows--;
      tifr2 |= (1<<1); //resetting the flag bit.
    }
  }

    //turn the timer off, 
      tccr2b &= ~(1<<0);
      tccr2b &= ~(1<<1);
      tccr2b &= ~(1<<2); 
}









void setup() {
  ddrb |= (1<<0) ;//setting the 0th bit of ddrb to 1;
}

void loop() {
  portb |= (1<<0);
  sleepself_extended(1500);
  portb &= ~(1<<0);
  sleepself_extended(1500);
}
