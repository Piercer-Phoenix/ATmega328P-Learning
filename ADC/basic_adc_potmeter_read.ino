#define prr *((volatile byte*) 0x64)
#define ddrd *((volatile byte*) 0x2A)
#define portd *((volatile byte*) 0x2B)
#define tcnt0 *((volatile byte*) 0x46)
#define tccr0a *((volatile byte*) 0x44)
#define tccr0b *((volatile byte*) 0x45)
#define ocr0a *((volatile byte*) 0x47)


#define adcsra *((volatile byte*) 0x7A)
#define adcsrb *((volatile byte*) 0x7B)
#define admux *((volatile byte*) 0x7C)
#define adch *((volatile byte*) 0x79)
#define adcl *((volatile byte*) 0x78)









/*   Order of things: 
 single run code: ( setup_ADC() function)
  single run code for setting up the ADc peripheral: 
 1) Starting the ADC by clearing (writing 0 to) the PRADC bit in the PRR register. 
 2) Setting the Prescaler for the ADC clock config.  register : ADCSRA
 3) Choose an analog input channel. in this case, A0.  ADMUX register, bits MUX3:0 set to 0-0-0
 4) set the bits to configure the ADC into free running mode
 5) Set the ADSC bit in the ADCSRA register, to begin the conversion process. 
 6) enable interrupt for the conversion coplete bit so that i can write an ISR, 
 7) Select a reference voltage for the conversion process, in this case,  will be the VCC. 
 8) enable the ADC by using the ADEN bit, that is SET it, that is, make it 1. 

 single run code to setup the timer0 module fior pwm signal generation (setup_timer0 functuion):
 1) Enable the timer0 module by clearing the PRTIM0 bit in the PRR register
 2) Setting the prescaler 
 3) selecting FAST PWM mode, mode 3 of the all WGM available, where top is 0xFF. 
 4) Set the COMOA1:0 bits to 1-0(binary form) for selcting non inverting mode. (remember that we are using fast pwm mode). and know that the
 output pin is automaticallyu selected as we have configured the OCR0A register and the COMO0A bits. 
 5) initiate the timer tcnt0 register to 0 and the OCR0A to a random value for the setup code. 


  pin setup: 
  1) set the PORTD4 pin as ouput(for led)

 dynamic code(code which has to run multiple times, basically, the ISR and the main loop) :
 1) Writing an ISR to read the ADCH/ADCL rregisters and store it in a global variable pot_read, which is used in the main loop
 by scaling it to the range of 0-255, and setting the OCR0A register as the pot_read value. 
 2) The main loop is a consumer which reads  the pot_read variable, scales it down, and writes the scaled value to the OCR0A register for altering 
 the PWM signal going to the LED.  and turn the led off if the pot meter reading is down 0. 
*/


volatile uint16_t pot_read = 0;




void setup_ADC(){
  //step 1: Disabling the power reduction mode for ADC using the PRR(0th bit control the ADC)
  prr &= ~(1<<0); 

  //step 2: setting the prescaler bits to divide the clock source to the ADC. This will be set it 128, by writing 1-1-1 to ADSP0:2 bits, 
  //which ar found in the ADCSRA register.

  adcsra |= (1<<0);
  adcsra |= (1<<1);
  adcsra |= (1<<2);

  //step 3: setting the analog input channel selectin using the ADMUX register. setting the MUX3:0 bits to 0-0-0-0.
  admux &= ~(1<<0);
  admux &= ~(1<<1);
  admux &= ~(1<<2);
  admux &= ~(1<<3);

  //step 4:configure ADC into free running mode using the ADTS2:0 bits in the adcsrb register, writing 0-0-0 to it. 

  adcsrb &= ~(1<<0);
  adcsrb &= ~(1<<1);
  adcsrb &= ~(1<<2);

  //step 6: activating the interurpts for the ADIF flag,(which is set when a conversion is complete.)
  adcsra |= (1<<3);

  //step 7: 
  admux |= (1<<6);
  admux &= ~(1<<7);


  //making sure the poutput is right shifted, by writing 0 to adlar, 
  admux &= ~(1<<5);

  //enabling the auto trigger bit

  adcsra |= (1<<5);

  //step 8:
  adcsra |= (1<<7);

  //step 5:
  adcsra |= (1<<6);

  


}



void setup_timer0(){
  //step 1: clear the PRTIM0 bit to enable the timer
  prr &= ~(1<<5);

  //step 2: setting the cs01:00 bits and clear cs02 bit, using the tccr0b register:

  tccr0b |= (1<<0);
  tccr0b |= (1<<1);
  tccr0b &= ~(1<<2);


  //step 3: Enabling the fast PWM mode, specifically mode 3,  of operation by setting the WGM bits in the tccr0x registers
  tccr0a |= (1<<0);  
  tccr0a |= (1<<1);  
  tccr0b &= ~(1<<3);


  // step4: settiing thhe corresponding com0a bits. COM0A1 = 1, COM0A0 = 0
  tccr0a |= (1<<7);
  tccr0a &=  ~(1<<6);

  //step 5: 
  tcnt0 = 0;
}



ISR(ADC_vect){
  pot_read  =  adcl | (adch << 8);
}



void setup() {
  setup_ADC();
  setup_timer0();

  //set LED as output
  ddrd |= (1<<6); //OC0a pin

}

void loop() {
uint32_t scaled_value = ((uint32_t)pot_read * 255) / 1024;
  ocr0a = scaled_value; 
}