#include <avr/io.h>
#include <util/delay.h>
#include "pinDefines.h"
#include "USART.h"
#include <avr/interrupt.h>
#include <util/twi.h>

#define M168_ADDRESS 0x04

/*

//SLA+W received, ACK returned 
TW_SR_SLA_ACK           0x60

//data received, ACK returned 
TW_SR_DATA_ACK          0x80

//data received, NACK returned
TW_SR_DATA_NACK         0x88

// stop or repeated start condition received while selected
TW_SR_STOP              0xA0

//no state information available
TW_NO_INFO              0xF8
*/


//INITIALIZE AS SLAVE

void I2C_init(uint8_t address){
 TWAR = address;
 TWCR = (1<<TWIE)| (1<<TWEA) | (1<<TWEN) | (1<<TWINT);
  printString("Initializing AVR as slave \r\n");
}

void I2C_stop(void){
 //some code to stop?
}

 ISR(TWI_vect){

 uint8_t packet; 

 //acknowledge avr my own address
 if((TWSR & TW_NO_INFO) == TW_SR_SLA_ACK){

  printString("Address Acknolwedged\r\n");

  TWCR = (1<<TWIE)| (1<<TWEA) | (1<<TWEN) | (1<<TWINT);

 }else if((TWSR & TW_NO_INFO) == TW_SR_DATA_ACK){
  packet = TWDR; 
  TWCR = (1<<TWIE)| (1<<TWEA) | (1<<TWEN) | (1<<TWINT);
  printString("Packet Received\r\n");
  printByte(packet);
  printString("\r\n"); 

  //if receive a "9", exit
  if(packet == 9){
   TWCR = (1<<TWIE)| (0<<TWEA) | (1<<TWEN) | (1<<TWINT);
   printString("9 Received, NACK\r\n");
  }
  } 
   
 }
int main(void){
 //initalize USART
 initUSART();
 //initialize as I2C slave<F10>                                                
 printString("==============I2C Test =================\r\n");
 I2C_init(M168_ADDRESS);

 //set / allow interrpts
 sei();
 DDRD |= (1<<PD2); 	//LED PORT D2 TO OUTPUT

}
