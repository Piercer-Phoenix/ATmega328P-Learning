#define portb *((volatile byte*) 0x25) 
#define pinb *((volatile byte*) 0x23) 
#define ddrb *((volatile byte*) 0x24) 



void setup() {
  //configuring PB0 as input
  ddrb &= ~(1<<0);
  portb |= (1<<0); //activating internal pull up
  ddrb |= (1<<4);
}

void loop() {
  int status = (pinb & (1<<0)); //status will be 0 if button is pressed. 
  // if(!status){
  //   portb |= (1<<4);
  // }
  // else{
  //   portb &= ~(1<<4);
  // }
  if(!status){
    portb ^= (1<<4); // tioggling the portb4 if there is a button press. 
  }
}
