#define ddrb *((volatile byte*) 0x24)
#define portb *((volatile byte*) 0x25) 
#define tccr0a *((volatile byte*) 0x44)
#define tccr0b *((volatile byte*) 0x45)
#define tcnt0 *((volatile byte*)0x46)
#define ocr0a *((volatile  byte*) 0x47)
#define tifr0 *((volatile byte* ) 0x35)







void sleepself(){
  //in this particular iteration i will be writing the code for using ctc mode and using the ocr0a register for the comprison.
  // the math is predefined in my notes, but i will be using prescaler of 1024, and thetop value oof the ocr0a will be 250, and the ocf0a will
  // be checked and set to 0 63 times, and once it hits  the total 63 times mark, the function will stop, before returning though, we will be stopping the clock by clearing 
  // cs02:00 bits.

  //step 1: Enabling CTC mode,
  tccr0b &= ~(1<<3);//  WGM02
  tccr0a |= (1<<1);
  tccr0a &= ~(1<<0);
  //step1 over.

  //step2:
  ocr0a = 250;
  tcnt0 = 0;

  int cnt = 0;
  int status = 0;
  ///step3:
  //clear the bits first
  tccr0b &= ~(1<<2);
  tccr0b &= ~(1<<1);
  tccr0b &= ~(1<<0);
  //set 002 and 00 bits.

  tccr0b |= (1<<2);
  tccr0b |= (1<<0);

  //step3 complete, which means timer is now running.
  
  
  
  //step4:
  //counter is already initialized before, to save even some time. 


  while(cnt <= 62){
    status = tifr0 & (1<<1);
    if(status){
      cnt++;
      tifr0 |= (1<<1);
    }    
  }


  //exiting the poll loop:
  //final step, to clear cs bits

  tccr0b &= ~(1<<2);
  tccr0b &= ~(1<<1);
  tccr0b &= ~(1<<0);


}


void setup() {
// in this example, ill be writing to pin number 8 for the arduino uno, which is.... after looking it up is..... PORTb's 0th bit. cool.
ddrb = 1; //setting DDRB'1 0th bit to 1,that is, making it output.
}


void loop() {
  portb = 1;
  //writing 1 to portb's 0th bit.
  sleepself();
  portb = 0;
  sleepself();
}
