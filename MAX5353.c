// MAX5353.c
// Runs on LM4F120/TM4C123
// Use SSI0 to send a 16-bit code to the MAX5353.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// SSI0Clk (SCLK, pin 4) connected to PA2
// SSI0Fss (!CS, pin 2) connected to PA3
// SSI0Tx (DIN, pin 3) connected to PA5
// see Figure 7.19 for complete schematic

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "MAX5353.h"

#define guitar 0
#define trumpet 1
#define bassoon 2

extern uint32_t instrument;
const uint16_t guitar_wave[64] = {
	4096, 4096, 4104, 4004, 3740, 3332, 2920, 2588, 2476, 2664, 3128, 3856, 4688, 5348, 5844, 6144, 
	6232, 6012, 5584, 4940, 4160, 3456, 3020, 3032, 3508, 4348, 5432, 6452, 7156, 7384, 7088, 6500, 
	5816, 5140, 4692, 4384, 4208, 4136, 4108, 4144, 4316, 4548, 4736, 4988, 5056, 5000, 4728, 4324, 
	3896, 3460, 3204, 3116, 3240, 3376, 3436, 3436, 3404, 3396, 3500, 3688, 3908, 4096, 4096, 4096
};
const uint16_t trumpet_wave[64] = {
	3948, 4196, 4360, 4440, 4536, 4640, 4556, 4368, 4280, 4168, 4140, 4116, 4032, 4264, 4600, 4680, 
	4348, 3660, 2716, 1488, 604, 2232, 4056, 4980, 5040, 4580, 4252, 3936, 3736, 3840, 4108, 4308, 
	4324, 4296, 4256, 4168, 4040, 3896, 3872, 3896, 3976, 4156, 4376, 4516, 4500, 4368, 4224, 4224, 
	4328, 4236, 4184, 4232, 4244, 4180, 4136, 4200, 4376, 4448, 4368, 4252, 4212, 4260, 4208, 3968
};
const uint16_t bassoon_wave[64] = {
	4272, 4676, 4700, 4644, 4520, 4452, 4408, 4304, 4128, 3940, 3852, 3948, 4328, 5372, 6948, 7452, 
	6300, 4124, 2152, 1236, 1320, 1888, 2504, 3228, 4152, 5080, 5680, 5844, 5500, 4804, 4020, 3276, 
	2632, 2128, 1984, 2376, 3216, 4220, 4992, 5292, 4932, 4196, 3580, 3304, 3304, 3400, 3448, 3444, 
	3596, 3844, 4024, 4092, 4184, 4368, 4708, 4896, 4744, 4532, 4392, 4408, 4436, 4304, 4108, 4012
};

//********DAC_Init*****************
// Initialize Max5353 12-bit DAC
// inputs:  initial voltage output (0 to 4095)
// outputs: none
// assumes: system clock rate less than 20 MHz
void DAC_Init(){
  SYSCTL_RCGCSSI_R |= 0x01;       // activate SSI0
  SYSCTL_RCGCGPIO_R |= 0x01;      // activate port A
  while((SYSCTL_PRGPIO_R&0x01) == 0){};// ready?
  GPIO_PORTA_AFSEL_R |= 0x2C;     // enable alt funct on PA2,3,5
  GPIO_PORTA_DEN_R |= 0x2C;       // configure PA2,3,5 as SSI
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFF0F00FF)+0x00202200;
  GPIO_PORTA_AMSEL_R = 0;         // disable analog functionality on PA
  SSI0_CR1_R = 0x00000000;        // disable SSI, master mode
  SSI0_CPSR_R = 0x02;             // 8 MHz SSIClk 
  SSI0_CR0_R &= ~(0x0000FF70);    // SCR = 0, SPH = 1, SPO = 0 Freescale
	SSI0_CR0_R |= 0x00000080;
  SSI0_CR0_R |= 0x0F;             // DSS = 16-bit data
  SSI0_DR_R = 0;               // load 'data' into transmit FIFO
  SSI0_CR1_R |= 0x00000002;       // enable SSI

}



//********DAC_Out*****************
// Send data to Max5353 12-bit DAC
// inputs:  voltage output (0 to 4095)
// outputs: none
void DAC_Out(int16_t note0, int16_t note1){   
  while((SSI0_SR_R&0x00000002)==0){};// SSI Transmit FIFO Not Full
	switch(instrument) {
		case guitar:
			if (note0 == -1) { // case where note0 is not currently assigned to anything
				if (note1 == -1) { // case where there is a pause between notes
					SSI0_DR_R = 0;
				}
				else {
					SSI0_DR_R = guitar_wave[note1]; // only note 1
				}
			}
			else {
				if (note1 == -1) {
					SSI0_DR_R = guitar_wave[note0];
				}
				else {
					SSI0_DR_R = guitar_wave[(note0 + note1) / 2];  //case where you add and then average them to create the proper note
				}
			}
			break;
		case trumpet:
			if (note0 == -1) {  // case where note0 is not currently assigned to anything
				if (note1 == -1) { //// case where there is a pause between notes
					SSI0_DR_R = 0;
				}
				else {
					SSI0_DR_R = trumpet_wave[note1];  // only note 1
				}
			}
			else {
				if (note1 == -1) {
					SSI0_DR_R = trumpet_wave[note0];
				}
				else {
					SSI0_DR_R = trumpet_wave[(note0 + note1) / 2];
				}
			}
			break;
		case bassoon:
			if (note0 == -1) {
				if (note1 == -1) {
					SSI0_DR_R = 0;
				}
				else {
					SSI0_DR_R = bassoon_wave[note1];
				}
			}
			else {
				if (note1 == -1) {
					SSI0_DR_R = bassoon_wave[note0];
				}
				else {
					SSI0_DR_R = bassoon_wave[(note0 + note1) / 2];
				}
			}
			break;
	}
  //SSI0_DR_R = code;                // data out, no reply
}
  
//********DAC_Out*****************
// Send data to Max5353 12-bit DAC
// inputs:  voltage output (0 to 4095)
// outputs: reply is returned
// send the 16-bit code to the SSI, return a reply
uint16_t DAC_Out2(uint16_t code){   uint16_t receive;
  while((SSI0_SR_R&0x00000002)==0){};// SSI Transmit FIFO Not Full
  SSI0_DR_R = code;                  // data out
  while((SSI0_SR_R&0x00000004)==0){};// SSI Receive FIFO Not Empty
  receive = SSI0_DR_R;               // acknowledge response
  return receive;
}

void DAC_InitRaw(void){
  SYSCTL_RCGCSSI_R |= 0x01;       // activate SSI0
  SYSCTL_RCGCGPIO_R |= 0x01;      // activate port A
  while((SYSCTL_PRGPIO_R&0x01) == 0){};// ready?
  GPIO_PORTA_AFSEL_R |= 0x24;     // enable alt funct on PA2,5, regular GPIO on PA3
  GPIO_PORTA_DEN_R |= 0x2C;       // configure PA2,5 as SSI, PA3 is regular GPIO
  GPIO_PORTA_DATA_R |= 0x08;      // PA3 FSS high
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFF0F00FF)+0x00200200;
  GPIO_PORTA_AMSEL_R = 0;         // disable analog functionality on PA
  SSI0_CR1_R = 0x00000000;        // disable SSI, master mode
  SSI0_CPSR_R = 0x02;             // 8 MHz SSIClk 
  SSI0_CR0_R &= ~(0x0000FFF0);    // SCR = 0, SPH = 0, SPO = 0 Freescale
  SSI0_CR0_R |= 0x0F;             // DSS = 16-bit data
  SSI0_CR1_R |= 0x00000002;       // enable SSI

}
//********DAC_Out*****************
// Send data to Max5353 12-bit DAC
// inputs:  voltage output (0 to 4095)
// outputs: none
// send the 16-bit code to the SSI, 
void DAC_OutRaw(uint16_t code){  
  GPIO_PORTA_DATA_R &= ~0x08;        // PA3 FSS low
  
  while((SSI0_SR_R&0x00000002)==0){};// SSI Transmit FIFO Not Full
  SSI0_DR_R = code;                  // data out

  GPIO_PORTA_DATA_R |= 0x08;         // PA3 FSS high
}
