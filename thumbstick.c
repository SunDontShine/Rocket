#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"
#include "USART.h"


//STATES
static uint8_t xneg,xpos,yneg,ypos,jbutton;

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

//CHECK STATE OF JOYSTICK BUTTON SWITCH ON/OF
static void checkJButtonState(){
 if(bit_is_clear(PINC,PC2) && jbutton == 0){
  jbutton = 1;
  printString("Button Pressed\r\n"); 
 PORTB |= 0x0f; //ALL LEDS ON; 
 }else if(!(bit_is_clear(PINC,PC2)) && jbutton == 1 ){
  PORTB &= 0xf0; //ALL LEDS OFF;
  jbutton = 0;
 }
}

static void checkYState(uint16_t axis){
if(axis == 0 && ypos == 0){
  PORTB |= (1<<PB3); //PB3 ON Y + (RED) 
  ypos = 1;
}else if(axis != 0 && ypos ==1){
 PORTB ^= (1<<PB3); //PB3 OFF Y + (RED) 
 ypos = 0;
 }

if(axis == 1023 && yneg == 0) {
  PORTB |= (1<<PB2); //PB2 ON Y - (RED) 
  yneg = 1;
}else if(axis != 1023 && yneg == 1){
 PORTB ^= (1<<PB2); //PB2 OFF Y - (RED) 
 yneg = 0;
 }
}

static void checkXState(uint16_t axis){
if(axis == 0 && xpos == 0){
  PORTB |= (1<<PB0); //PB0 ON X + (BLUE) 
  xpos = 1;
}else if(axis !=0 && xpos == 1){
 PORTB ^=(1<<PB0);   //PB0 OFF X + (BLUE)
 xpos = 0;
}

if(axis == 1023 && yneg ==0){
  PORTB |= (1<<PB1);  //PB1  ON X - (BLUE)
  xneg = 1;
} else if(axis != 1023 && xneg == 1){
  PORTB ^=(1<<PB1);    //PB1 OFF X - (BLUE)
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
 
checkJButtonState();
checkYState(yaxis);
checkXState(xaxis);
/*
if(yaxis == 0){
  yposOn();
}else if(yaxis == 1023){
  ynegOn();
 }


 if(xaxis == 1023){
  xposOn();
 }else if(xaxis == 0){
  xnegOn();
  }
*/
 } 
}
