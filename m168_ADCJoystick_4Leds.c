#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"
#include "USART.h"

//STATES
static uint8_t xneg; //PB0
static uint8_t xpos; //PB7
static uint8_t yneg; //PB6
static uint8_t ypos; //PB5
static uint8_t jbutton; //ALL ABOVE PORTS

//INITIALIZE ADC
void initADC(){
 ADMUX|=(1<<REFS0);	//REFERENCE VOLTAGE ON AVCC PIN
 ADCSRA|=(1<<ADPS1)|(1<<ADPS0); //ADC CLOCK PRESCALER /8
 ADCSRA|=(1<<ADEN);	//ENABLE ADC
}

static void checkJButtonState(){
 if(bit_is_clear(PINC,PC2) && jbutton == 0){
  jbutton = 1;
  printString("Button Pressed\r\n"); 
  PORTB |= (1<<PB0)|(1<<PB6)|(1<<PB7)|(1<<PB5); //ALL LEDS ON; 
 }else if(!(bit_is_clear(PINC,PC2)) && jbutton == 1 ){//LEDS OFF
    if(xneg == 0)//TURNS OFF LEDS THAT ARE IN STATE "ZERO"
	PORTB ^= (1<<PB0);
    if(xpos == 0)
	PORTB ^=(1<<PB7);
    if(yneg == 0)
	PORTB ^=(1<<PB6);
    if(ypos == 0)
	PORTB ^=(1<<PB5);
    jbutton = 0;
 }
}

static void checkYState(uint16_t axis){
if(axis == 0 && ypos == 0){
  PORTB |= (1<<PB5); //PB5 ON Y + (RED) 
  ypos = 1;
}else if(axis != 0 && ypos ==1){
 PORTB ^= (1<<PB5); //PB5 OFF Y + (RED) 
 ypos = 0;
 }

if(axis == 1023 && yneg == 0) {
  PORTB |= (1<<PB6); //PB6 ON Y - (RED) 
  yneg = 1;
}else if(axis != 1023 && yneg == 1){
 PORTB ^= (1<<PB6); //PB6 OFF Y - (RED) 
 yneg = 0;
 }
}

static void checkXState(uint16_t axis){
if(axis == 1023 && xpos == 0){
  PORTB |= (1<<PB7); //PB7 ON X + (BLUE) 
  xpos = 1;
}else if(axis !=1023 && xpos == 1){
 PORTB ^=(1<<PB7);   //PB7 OFF X + (BLUE)
 xpos = 0;
}

if(axis == 0 && xneg ==0){
  PORTB |= (1<<PB0);  //PB0  ON X - (BLUE)
  xneg = 1;
} else if(axis != 0 && xneg == 1){
  PORTB ^=(1<<PB0);    //PB0 OFF X - (BLUE)
  xneg = 0;
 }
}
//Sets MUX PCn Pin for input, start conversion, waits& return result
 uint16_t readADC(uint8_t channel){
 ADMUX = (0xf0 & ADMUX) | channel; //SET PCn ON MUX
 ADCSRA |= (1<<ADSC);		//BEGIN SAMPLING/CONVERSION
 loop_until_bit_is_clear(ADCSRA,ADSC); //WAIT FOR RESULT
 return (ADC);
}



int main(void){
// ----- Initialize ----- //
 uint16_t xaxis; //PC0 PIN
 uint16_t yaxis; //PC1 PIN
 
/*---------------------PWM----------------------*/

 while(1){

  initADC();
  DDRB |= (1<<PB7)|(1<<PB6)|(1<<PB0)|(1<<PB5); //SET LED PINS TO OUTPUT
  PORTC |= (1<<PC2); //ENABLE PULL-UP RESISTOR (INPUT BY DEFAULT)

  xaxis = readADC(PC0);
  yaxis = readADC(PC1);
 
  _delay_ms(50);
  checkJButtonState();
  checkYState(yaxis);
  checkXState(xaxis);
 }
 return(0);
}

