#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"
#include "USART.h"

//INITIALIZE ADC
void initADC(){
 ADMUX|=(1<<REFS0);	//REFERENCE VOLTAGE ON AVCC PIN
 ADCSRA|=(1<<ADPS1)|(1<<ADPS0); //ADC CLOCK PRESCALER /8
 ADCSRA|=(1<<ADEN);	//ENABLE ADC
}

//INITIALIZE JOYSTICK SWITCH
void initJoystickSwitch(){
 PORTC |= (1<<PC2); //ENABLE PULL-UP RESISTOR (INPUT BY DEFAULT)
}
//Sets MUX PCn Pin for input, start conversion, waits& return result
uint16_t readADC(uint8_t channel){
 ADMUX = (0xf0 & ADMUX) | channel; //SET PCn ON MUX
 ADCSRA |= (1<<ADSC);		//BEGIN SAMPLING/CONVERSION
 loop_until_bit_is_clear(ADCSRA,ADSC); //WAIT FOR RESULT
 return (ADC);
}

//SWITCH LEDS ON/OFF WHEN JOYSTICK PRESSED
void joystickOn(){
 PORTB |= 0x0f;
}

//X + (BLUE) 
void xposOn(){
 PORTB |= (1<<PB1); //Turn on PB1
}
//X - (BLUE) 
void xnegOn(){ 
 PORTB |= (1<<PB0); //Turn on PB0
}
//Y + (RED) 
void yposOn(){
 PORTB |= (1<<PB3); //Turn on PB3
}
//Y - (RED)
void ynegOn(){
 PORTB |= (1<<PB2); //Turn on PB2
}


int main(void){
// ----- Initialize ----- //
 uint16_t yaxis; //PC1 PIN
 uint16_t xaxis; //PC0 PIN
 uint16_t swbutton; //PC2 PIN

 initADC();
 initUSART();
 DDRB |= 0x0f; //SET LED PINS TO OUTPUT
 initJoystickSwitch();

 while(1){
 xaxis = readADC(PC0);
 yaxis = readADC(PC1);
 
 printString("Y-AXIS : ");
 //yaxis = readADC(PC1);
 printWord(yaxis);
 printString("\r\n");

 printString("X-AXIS : " );
 //xaxis = readADC(PC0);
 printWord(xaxis);
 printString("\r\n");
 
 if(bit_is_clear(PINC,PC2)){
  printString("Button Pressed\r\n"); 
  joystickOn();
  _delay_ms(100);
 }

if(yaxis == 0){
  yposOn();
}

if(yaxis == 1023){
  ynegOn();
 }

  if(xaxis == 1023){
  xposOn();
 }else if(xaxis == 0){
  xnegOn();
 }

_delay_ms(100);
PORTB &= 0xf0; //turn off leds
 } 
}
