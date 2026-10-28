#define ubbr0l *((volatile byte*) 0xC4)
#define ucsr0a *((volatile byte*) 0xC0)
#define ucsr0b *((volatile byte*) 0xC1)
#define ucsr0c *((volatile byte*) 0xC2)
#define udr0 *((volatile byte*) 0xC6)
#define ubbr0h *((volatile byte*) 0xC5)
#define portb *((volatile byte*) 0x25)
#define ddrb *((volatile byte*) 0x24)



void usart_setup(){
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
  ubbr0l = 103;
  ubbr0h = 0;
  

  //step 4: enabling the reciever:
  ucsr0b |= (1<<4);

}

uint8_t usart_read(){
  while(! (ucsr0a & (1<<7)));
  return udr0;
}


void setup(){
  usart_setup();
  ddrb |= (1<<0);
  ddrb |= (1<<1);
}

void loop(){
  uint8_t op = usart_read();

  if(op == '1'){
    portb |= (1<<0);
    portb &= ~(1<<1);
  }
  else if(op == '2'){
    portb &= ~(1<<0);
    portb |= (1<<1);
  }
}