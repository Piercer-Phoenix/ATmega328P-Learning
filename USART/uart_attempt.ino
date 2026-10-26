#define ubbr0l *((volatile byte*) 0xC4)
#define ucsr0a *((volatile byte*) 0xC0)
#define ucsr0b *((volatile byte*) 0xC1)
#define ucsr0c *((volatile byte*) 0xC2)
#define udr0 *((volatile byte*) 0xC6)
#define ubrr0h *((volatile byte*) 0xC5)


void usart_setup(){
  //enable the trasnsmitting enable bit 
  ucsr0b |= (1<<3);
  //set the baud rate for the operation
  ubbr0l = 103;
  ubrr0h = 0;
  //select the mode of operation as asynchornous 
  ucsr0c &= ~(1<<7);
  ucsr0c &= ~(1<<6);
  //seecting the frame foormat
  // 1) selcting the parity bits. 
  ucsr0c &= ~(1<<5);
  ucsr0c &= ~(1<<4);
  // 2) stop bit config
  ucsr0c &= ~(1<<3);
  // 3) character size. 
  ucsr0b &= ~(1<<2);
  ucsr0c |= (1<<1);
  ucsr0c |= (1<<0); 
}


void write_usart(uint8_t data){
  while (!(ucsr0a & (1<<5)));   // wait until UDRE0 is set
  udr0 = data;
}



void setup() {
  usart_setup();

}

void loop() {
  write_usart(65);
  delay(1000);
}
