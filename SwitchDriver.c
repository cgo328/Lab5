#include "SwitchDriver.h"
#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

uint32_t falledge;
extern char alarmon;
// Initializes Port to have falling edge triggers
void EdgeTrigger_Init(){
	SYSCTL_RCGCGPIO_R |= 0x20;									// activate clock for Port F
	while((SYSCTL_RCGCGPIO_R & 0x20) == 0) {}		// wait for clock
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R |= 0x11;
	GPIO_PORTF_AFSEL_R &= ~(0x11);
	GPIO_PORTF_DIR_R &= ~0x17;									// make PF4, PF2-0 in
	GPIO_PORTF_DEN_R |= 0x17;										// enable digital I/O on PF4, PF2-0
	GPIO_PORTF_PUR_R |= 0x11;										// enable pull down resistors on PF4 and PF0	
	GPIO_PORTF_IS_R &= ~0x17;										// PF4, PF2-0 are edge sensitive
	GPIO_PORTF_IBE_R &= ~0x17;									// PF4, PF2-0 are not both edges
	GPIO_PORTF_IEV_R &= ~0x17;									// PF4, PF2-0 falling edge event
	GPIO_PORTF_ICR_R = 0x17;										// 
	GPIO_PORTF_IM_R |= 0x17;
	NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00600000;	// priority 3
	NVIC_EN0_R = 0x40000000;
	falledge = 0;
}

// ISR for button presses
void GPIOPortF_Handler(void){

}
