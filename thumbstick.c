#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"
#include "USART.h"
//initial

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
_delay_ms(1000);
PORTB &= 0x00;
}

//X + (BLUE)
void xposOn(){
 PORTB &= 0x00;
 PORTB |= 0x01; //Turn on PB0
 PORTB &= 0x00;
}
//X - (BLUE)
void xnegOn(){
 PORTB &= 0x00;
 PORTB |= 0x02; //Turn on PB1
PORTB &= 0x00;
}
//Y + (RED)
void yposOn(){
 PORTB &= 0x00;
 PORTB |= 0x03; //Turn on PB2
}
//Y - (RED)
void ynegOn(){
 PORTB &= 0x00;
 PORTB |= 0x04; //Turn on PB3
}


int main(void){
// ----- Initialize ----- //
 uint16_t xaxis;
 uint16_t yaxis;
 uint16_t swbutton; 

 initADC();
 initUSART();
 DDRB |= 0b00001111; //SET LED PINS TO OUTPUT
 initJoystickSwitch();

 while(1){
 if(bit_is_clear(PINC,PC2)){
  printString("Button Pressed\r\n"); 
 }
 printString("TESTING\r\n");
 _delay_ms(500);
 printByte(swbutton);
 }
}
