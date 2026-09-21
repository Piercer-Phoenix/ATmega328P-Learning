#define prr *((volatile byte*) 0x64)
#define ddrd *((volatile byte*) 0x2A)
#define portd *((volatile byte*) 0x2B)
#define tcnt0 *((volatile byte*) 0x46)
#define tccr0a *((volatile byte*) 0x44)
#define tccr0b *((volatile byte*) 0x45)
#define ocr0a *((volatile byte*) 0x47)


/*Order of the things in the code: 
NOTE: This code will be using the timer0 module, using the OC0A pin, which corresponds to the PD6 bit, which maps to the digital pin 6 on the arduino
UNO board. 
1) set COM0A bits, for enabling non inverted mode.
2) enable the timer module using the PRR resgiter and the PRTIM0 bit
3) set the WGM bits to mode 3 / binary : 0-1-1
4) selet the prescaler by setting the CS bits to 0-1-1, since we are using a prescaler of 64. 
5) init tcnt0 to 0 and the ocr0a to the val of the function.

*/





void noninv_analogwrite(byte val){
  // step1: settiing thhe corresponding com0a bits. COM0A1 = 1, COM0A0 = 0
  tccr0a |= (1<<7);
  tccr0a &=  ~(1<<6);

  //step 2: clear the PRTIM0 bit to enable the timer
  prr &= ~(1<<5);

  //step 3: Enabling the fast PWM mode, specifically mode 7,  of operation by setting the WGM bits in the tccr0x registers
  tccr0a |= (1<<0);  
  tccr0a |= (1<<1);  
  tccr0b &= ~(1<<3);

  //step 4: setting the cs01:00 bits and clear cs02 bit, using the tccr0b register:

  tccr0b |= (1<<0);
  tccr0b |= (1<<1);
  tccr0b &= ~(1<<2);


  //step 5: init the tcnt0 to 0 and ocr0a value
  ocr0a = val;
  tcnt0 = 0;

}






void setup() {
  // since I am using thhe OC0A pin, which is the PD6 pin, I have to set the 7th bit
  ddrd |= (1<<6);
}

void loop() {
  noninv_analogwrite(128);
  delay(1000); //for the sake of simplicity of the code, i will not be using my own code for sleep. 
  noninv_analogwrite(255);
  delay(1000);
  noninv_analogwrite(0); 
  delay(1000);
  noninv_analogwrite(10); 
  delay(1000);
  // for(int i = 0;i <= 255; i += 3){
  //   noninv_analogwrite(i);
  //   delay(100);
  // }
}
