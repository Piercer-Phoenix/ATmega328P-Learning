//for timer0
#define tccr0a *((volatile byte*) 0x44)
#define tccr0b *((volatile byte*) 0x45)
#define timsk0 *((volatile byte*) 0x6E)
#define prr *((volatile byte*) 0x64)
#define ocr0a *((volatile byte*) 0x47)
#define tcnt0 *((volatile byte*) 0x46)
#define tifr0 *((volatile byte*) 0x35)

//for timer2
#define tcnt2 *((volatile byte*) 0xB2)
#define tccr2a *((volatile byte*) 0xB0)
#define tccr2b *((volatile byte*) 0xB1)
#define ocr2a *((volatile byte*) 0xB3)
#define ocr2b *((volatile byte*) 0xB4)
#define tifr2 *((volatile byte*) 0x37)
#define timsk2 *((volatile byte*) 0x70)

//for ADC
#define adcsra *((volatile byte*) 0x7A)
#define adcsrb *((volatile byte*) 0x7B)
#define admux *((volatile byte*) 0x7C)
#define adch *((volatile byte*) 0x79)
#define adcl *((volatile byte*) 0x78)

//for USART
#define ubrr0l *((volatile byte*) 0xC4)
#define ucsr0a *((volatile byte*) 0xC0)
#define ucsr0b *((volatile byte*) 0xC1)
#define ucsr0c *((volatile byte*) 0xC2)
#define udr0 *((volatile byte*) 0xC6)
#define ubrr0h *((volatile byte*) 0xC5)


//GPIO
#define portb *((volatile byte*) 0x25)
#define pinb *((volatile byte*) 0x23)
#define ddrb *((volatile byte*) 0x24)
#define ddrd *((volatile byte*) 0x2A)

void pwm_write(uint8_t val);   // forward declaration

volatile uint32_t tick_count = 0;
volatile uint16_t pot_read = 0;
volatile uint8_t cmd_byte = 0;


ISR(TIMER2_COMPA_vect){
  tick_count++;
}

ISR(ADC_vect){
  pot_read  =  adcl | (adch << 8);
  pot_read = (pot_read * 255) / 1024;
}

ISR(USART_RX_vect){
  uint8_t temp = udr0;
  if(temp == 'A' || temp == 'B'){
    cmd_byte = temp;
  }
}


typedef struct {
  uint32_t last;
  uint16_t duration;
  void (*task) (void);
} Task;


void setup_usart(){
  // step 1: selecting the mode of operation to asynchronous. 
  ucsr0c &= ~(1<<7);
  ucsr0c &= ~(1<<6);
//step 2: configuring the frame.
// a) disabling the parity: 
  ucsr0c &= ~(1<<5);
  ucsr0c &= ~(1<<4);
  //b) configuring the data bits
  ucsr0c |= (1<<1);
  ucsr0c |= (1<<2);
  ucsr0b &= ~(1<<2);
  //c)one stop bit:
  ucsr0c &= ~(1<<3);


  //step 3: Configuring the baud rate, ill be using a 9600 baud rate setup
  ubrr0l = 103;
  ubrr0h = 0;
  

  //step 4: enabling the reciever:
  ucsr0b |= (1<<4);
  //have to enable recieving complete interrupt too. 
  ucsr0b |= (1<<7);
}

void setup_timer2(){
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

void setup_timer0(){

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

  //connect oc0a
  tccr0a |= (1<<7);   // COM0A1 = 1
  tccr0a &= ~(1<<6);  // COM0A0 = 0  (non-inverting)


  //step 4: init the tcnt0 to 0 
  tcnt0 = 0;

}

void setup_adc(){
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

  //step 7: 
  admux |= (1<<6);
  admux &= ~(1<<7);

  //making sure the poutput is right shifted, by writing 0 to adlar, 
  admux &= ~(1<<5);

  //enabling the auto trigger bit
  adcsra |= (1<<5);

  //step 8:
  adcsra |= (1<<7);
   //step 6: activating the interurpts for the ADIF flag,(which is set when a conversion is complete.)
  adcsra |= (1<<3);

  //step 5:
  adcsra |= (1<<6);
}



void toggle_task(void){
  portb ^= (1<<4);
}


void adc_task(void){
  pwm_write(pot_read); //write to oc0a
}


void usart_task(void){
  if(cmd_byte == 'A'){
    portb |= (1<<0);
  }
  else if(cmd_byte == 'B'){
    portb &= ~(1<<0);
  }
}



//helper functions

void pwm_write(uint8_t val){
  ocr0a = val;
}

uint32_t millis_s(){
  return tick_count;
}


Task task_table[3] = {
  {0, 5000, toggle_task},
  {0, 50, usart_task},
  {0, 1000, adc_task}
};





void setup() {
  setup_timer2();
  setup_timer0();
  setup_usart();
  setup_adc();
  ddrb |= (1<<4);
  ddrb |= (1<<0);
  ddrd |= (1<<6);

}

void loop() {
  uint32_t now = millis_s();
  for(int i =0;i<3;i++){
    if(now - (task_table[i].last) >= (task_table[i].duration) ){
      //fire  the corresponding function of the task. 
      task_table[i].last = now;
      task_table[i].task();
    }
  }
}
