#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"
#include "USART.h"

#define ADC_MAX 1023
#define ADC_MIN 0
#define RIGHT_ROT_MIN 400
#define VERT_MAX_POS 414 
#define VERT_MIN_POS 12
#define LOWSPEED 8
#define MEDIUMSPEED 12
#define HIGHSPEED 18

static uint8_t jbutton;

//INITIALIZE ADC
void initADC(){
 ADMUX|=(1<<REFS0);		//REFERENCE VOLTAGE ON AVCC PIN
 ADCSRA|=(1<<ADPS1)|(1<<ADPS0); //ADC CLOCK PRESCALER /8
 ADCSRA|=(1<<ADEN);		//ENABLE ADC
}

//INITIALIZE PINS FOR PWM
void initPWM(){
DDRB |= (1<<PB1) | (1<<PB2);  		//OC1A,OC1B PINS TO OUTPUT
TCCR1A |= (1<<WGM10)| (1<<WGM11);	//FAST PWM, 10-BIT
TCCR1B |= (1<<WGM12);
TCCR1A |= (1<<COM1A1) | (1<<COM1B1);	//OC1A,OC1B PIN TO PWM
TCCR1B |= (1<<CS11);			//PRESCALER 8
}

//CHECK STATE OF JOYSTICK BUTTON SWITCH ON/OF
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
//SETS MUX PCn PIN FOR INPUT, START CONVERSION, WAIT & RETURN RESULT
 uint16_t readADC(uint8_t channel){
 ADMUX = (0xf0 & ADMUX) | channel; //SET PCn ON MUX
 ADCSRA |= (1<<ADSC);		//BEGIN SAMPLING/CONVERSION
 loop_until_bit_is_clear(ADCSRA,ADSC); //WAIT FOR RESULT
 return (ADC);
}

uint16_t rotateRight(uint16_t value){
 return ((ADC_MAX%value)+RIGHT_ROT_MIN);
}

int main(void){
// ----- Initialize ----- //
 uint16_t xaxis; 	//PC0 ADC VALUE
 uint16_t yaxis; 	//PC1 ADC VALUE 
 DDRB |= (1 <<PB0)|(1<<PB5)|(1<<PB6)|(1<<PB7);
 PORTC |= (1<<PC2); 	//THUMBSTICK PIN PULLUP RESISTOR SET

 initUSART();
 initADC();
 initPWM();
 OCR1B = VERT_MAX_POS;

 while(1){
  xaxis = readADC(PC0);
  yaxis = readADC(PC1);

  //X-AXIS 360 DEGREE ROTATION 
  if(xaxis == 514 || xaxis == 515)
	OCR1A = 1023;
  else if(xaxis < 514)
	OCR1A = 180;
  else
	OCR1A = rotateRight(xaxis);
  
  if(yaxis < 500 && OCR1B > VERT_MIN_POS){
	if(yaxis >=300)
		OCR1B -=LOWSPEED;
	else if(yaxis >=100)
		OCR1B -=MEDIUMSPEED;
	else
		OCR1B -=HIGHSPEED;

  }else if(yaxis > 530 && OCR1B < VERT_MAX_POS){
	if(yaxis <=700)
		OCR1B +=LOWSPEED;
	else if(yaxis <=900)
		OCR1B +=MEDIUMSPEED;
	else
		OCR1B +=HIGHSPEED;
   }
 _delay_ms(20); 
checkJButtonState();
}
 return(0);
 }
