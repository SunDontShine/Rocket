#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"
#include "USART.h"

#define ADC_MAX 1023
#define ADC_MIN 0
#define RIGHT_ROT_MIN 400

#define A_FRONT_B_BACK_LIMIT 80
#define A_BACK_B_FRONT_LIMIT 270

#define A_START_POSITION 140
#define B_START_POSITION 170
#define A_FRONT 250 //Servo A max front and back
#define A_BACK 30 
#define B_FRONT 70 //Servo B max front and back
#define B_BACK 290

#define SPEED 6

//static uint8_t jbutton;

//INITIALIZE ADC
void initADC(){
 ADMUX|=(1<<REFS0);		//REFERENCE VOLTAGE ON AVCC PIN
 ADCSRA|=(1<<ADPS1)|(1<<ADPS0); //ADC CLOCK PRESCALER /8
 ADCSRA|=(1<<ADEN);		//ENABLE ADC
}

//INITIALIZE PINS FOR PWM
//OCR1B is the "left" motor, OCR1A is the "right" motor"
void initPWM(){
DDRB |= (1<<PB1) | (1<<PB2);  		//OC1A,OC1B PINS TO OUTPUT
TCCR1A |= (1<<WGM10)| (1<<WGM11);	//FAST PWM, 10-BIT
TCCR1B |= (1<<WGM12);
TCCR1A |= (1<<COM1A1) | (1<<COM1B1);	//OC1A,OC1B PIN TO PWM
TCCR1B |= (1<<CS11);			//PRESCALER 8
}

//CHECK STATE OF JOYSTICK BUTTON SWITCH ON/OF
/*
static void checkJButtonState(){
 if(bit_is_clear(PINC,PC2) && jbutton == 0){
  jbutton = 1;
  printString("Button Pressed\r\n"); 
  PORTB |= (1<<PB0)|(1<<PB6)|(1<<PB7)|(1<<PB5); 	//ALL LEDS ON; 
 }else if(!(bit_is_clear(PINC,PC2)) && jbutton == 1 ){	//LEDS OFF
  	PORTB ^= (1<<PB0)|(1<<PB6)|(1<<PB7)|(1<<PB5); 
    	jbutton = 0;
 }
}
*/

//SETS MUX PCn PIN FOR INPUT, START CONVERSION, WAIT & RETURN RESULT
 uint16_t readADC(uint8_t channel){
 ADMUX = (0xf0 & ADMUX) | channel; //SET PCn ON MUX
 ADCSRA |= (1<<ADSC);		//BEGIN SAMPLING/CONVERSION
 loop_until_bit_is_clear(ADCSRA,ADSC); //WAIT FOR RESULT
 return (ADC);
}

//for 360 servo
/*
uint16_t rotateRight(uint16_t value){
 return ((ADC_MAX%value)+RIGHT_ROT_MIN);
}
*/

int main(void){
// ----- Initialize ----- //
 //uint16_t xaxis; 	//PC0 ADC VALUE
 uint16_t yaxis; 	//PC1 ADC VALUE 
 DDRB |= (1 <<PB0)|(1<<PB5)|(1<<PB6)|(1<<PB7);
 PORTC |= (1<<PC2); 	//THUMBSTICK PIN PULLUP RESISTOR SET

 initUSART();
 initADC();
 initPWM();
 OCR1A = A_START_POSITION;
 OCR1B = B_START_POSITION;

 while(1){
//  xaxis = readADC(PC0);
  yaxis = readADC(PC1);

/*
printString("Y axis :");
printWord(yaxis);
printString("\r\nOCR1A: ");
printWord(OCR1A);
printString("\r\nOCR1B: ");
printWord(OCR1B);
printString("\r\n");
*/
  //X-AXIS 360 DEGREE ROTATION 
/*
  if(xaxis == 514 || xaxis == 515)
	OCR1A = 1023;
  else if(xaxis < 514)
	OCR1A = 180;
  else
	OCR1A = rotateRight(xaxis);
 */ 
  //Joystick is rotated up (servos forward)
  if(yaxis < 500 && OCR1B > A_FRONT_B_BACK_LIMIT){
	OCR1A += SPEED;
	OCR1B -= SPEED;
  //Joystick is rotated down (servos backward)
  }else if(yaxis > 530 && OCR1B < A_BACK_B_FRONT_LIMIT){
	OCR1B += SPEED;
	OCR1A -= SPEED;
   }
 _delay_ms(20); 
//checkJButtonState();
}
 return(0);
 }
