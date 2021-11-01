#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>

#define BAUD 9600
unsigned char old_state;

void led_button_setup(){
  DDRD |= (1<<4);
  DDRC &= ~(1<<5);
  old_state = 0;
}
int rising_edge(){
   
  unsigned char current_state = PINC & (1<<5);
  int rising = (old_state == 0) && (current_state != 0);
  old_state = current_state; 
  return rising;
}
void light_switch(){
  PORTB ^= (1<<5);
}
void uart_setup(){
  //BAUDRATE
  UBRR0 = (F_CPU/16/BAUD) - 1;
  //speed - normal
  UCSR0A &= ~(1<<U2X0);
  //TRANSMISSION AND RECIVER
  UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
  //character size
  UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
  UCSR0B &= ~(1<<UCSZ02);
}
void uart_putbyte(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));

    // Send data by assigning into UDR0
    UDR0 = data;
}
int uart_getbyte(unsigned char *buffer) {
    // If receive buffer contains data...
    if (UCSR0A & (1 << RXC0)) {
        // Copy received byte from UDR0 into memory location (*buffer)
        *buffer = UDR0;
        // 
        return 1;
    }
    else {
        return 0;
    }
}

void uart_put_string(char* str){
  int i = 0;
  while(str[i] != 0){
    uart_putbyte(str[i]);
    i++;
}
  uart_putbyte(0);
}

void uart_get_string(char*str, int str_length){
  int i = 0; 
  unsigned char ch;
  while(1){
    while(!uart_getbyte(&ch)){}
    
    if(ch == 0){
      break;
  }
    else if( i < str_length-1){
      str[i] = ch;
      i++;
    }
    str[i] = 0;
  }
}

void transmission_content(){
  const int sender = 0; //this will be dertermined by how many people are playing
  if(sender){
    static char * content[] = {"Hello World", "Manly will win the premiership"};
    
    for(int i = 0; i<2; i++){
      uart_put_string(content[i]);
      _delay_ms(300); 
    }
  }
  else{
    char content[40];
    uart_get_string(content, 40);
    uart_put_string(content);
    
    uart_putbyte('\n');
  }
}



int main (void){
 
  
    led_button_setup();
  while(1){
    int rising = rising_edge();
    if (rising){
      light_switch();  
  }
  

  }  
  
  return 0;
}
             
             




