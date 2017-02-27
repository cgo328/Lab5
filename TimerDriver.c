#include "TimerDriver.h"
#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "MAX5353.h"

#define C_2 11945   // 65.406 Hz
#define DF_1 11274   // 69.296 Hz
#define D_1 10641   // 73.416 Hz
#define EF_1 10044   // 77.782 Hz
#define E_1 9480   // 82.407 Hz
#define F_1 8948   // 87.307 Hz
#define GF_1 8446   // 92.499 Hz
#define G_1 7972   // 97.999 Hz
#define AF_1 7525   // 103.826 Hz
#define A_1 7102   // 110.000 Hz
#define BF_1 6704   // 116.541 Hz
#define B_1 6327   // 123.471 Hz
#define C_1 5972   // 130.813 Hz
#define DF0 5637   // 138.591 Hz
#define D0 5321   // 146.832 Hz
#define EF0 5022   // 155.563 Hz
#define E0 4740   // 164.814 Hz
#define F0 4474   // 174.614 Hz
#define GF0 4223   // 184.997 Hz
#define G0 3986   // 195.998 Hz
#define AF0 3762   // 207.652 Hz
#define A0 3551   // 220.000 Hz
#define BF0 3352   // 233.082 Hz
#define B0 3164   // 246.942 Hz
#define C0 2986   // 261.626 Hz
#define DF 2819   // 277.183 Hz
#define D 2660   // 293.665 Hz
#define EF 2511   // 311.127 Hz
#define E 2370   // 329.628 Hz
#define F 2237   // 349.228 Hz
#define GF 2112   // 369.994 Hz
#define G 1993   // 391.995 Hz
#define AF 1881   // 415.305 Hz
#define A 1776   // 440.000 Hz
#define BF 1676   // 466.164 Hz
#define B 1582   // 493.883 Hz
#define C 1493   // 523.251 Hz
#define DF1 1409   // 554.365 Hz
#define D1 1330   // 587.330 Hz
#define EF1 1256   // 622.254 Hz
#define E1 1185   // 659.255 Hz
#define F1 1119   // 698.456 Hz
#define GF1 1056   // 739.989 Hz
#define G1 997   // 783.991 Hz
#define AF1 941   // 830.609 Hz
#define A1 888   // 880.000 Hz
#define BF1 838   // 932.328 Hz
#define B1 791   // 987.767 Hz
#define C1 747   // 1046.502 Hz
#define DF2 705   // 1108.731 Hz
#define D2 665   // 1174.659 Hz
#define EF2 628   // 1244.508 Hz
#define E2 593   // 1318.510 Hz
#define F2 559   // 1396.913 Hz
#define GF2 528   // 1479.978 Hz
#define G2 498   // 1567.982 Hz
#define AF2 470   // 1661.219 Hz
#define A2 444   // 1760.000 Hz
#define BF2 419   // 1864.655 Hz
#define B2 395   // 1975.533 Hz
#define C2 373   // 2093.005 Hz

#define guitar 0
#define trumpet 1
#define bassoon 2

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

typedef struct{
	int freq;
	int len;
} note;

extern note song0[100], song1[100];
extern uint32_t wave0, wave1, count0, count1, note0, note1, instrument;
extern char on_button, song_going;	

// Initialize Timer0 to trigger every second
void Timer0A_Init(uint32_t time){
	volatile uint32_t delay;
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0x00;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = time;         // start value based on frequency of note
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 1
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x20000000;
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}

// occurs based on 
void Timer0A_Handler(){
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;  // acknowledge timer0A timeout
	if (wave0 == 64) {
		wave0 = 0;
		if (count0 > song0[note0].len - 20) { // to insert the pauses between notes.
			if (count1 > song1[note1].len - 20) {
				DAC_Out(-1, -1);
			}
			else {
				DAC_Out(-1, wave1);
			}
		}
	else {
		if (count1 > song1[note1].len - 20) {
				DAC_Out(wave0, -1);
			}
			else {
				DAC_Out(wave0, wave1);
			}
		}
		if (count0 == song0[note0].len) {
			count0 = 0;
			note0++;
			if(note0 == 100){
				song_going = 0;
				disable_music();
				return;
			}
			TIMER0_TAILR_R = song0[note0].freq;
		}
	}
	wave0++;
	count0++;
}

// Initialize Timer0
void Timer1A_Init(uint32_t time){
	volatile uint32_t delay;
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x02;      // activate timer1
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER1_CTL_R &= ~TIMER_CTL_TAEN; // disable timer1A during setup
  TIMER1_CFG_R = 0x00;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER1_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER1_TAILR_R = time;         // start value for 1 Hz interrupts
  TIMER1_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// clear timer1A timeout flag
  TIMER1_CTL_R |= TIMER_CTL_TAEN;  // enable timer1A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer1A=priority 2
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00004000;
  NVIC_EN0_R = 1<<21;              // enable interrupt 21 in NVIC
}

// ISR for Timer0
void Timer1A_Handler(){
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;  // acknowledge timer0A timeout
	if (wave1 == 64) {
		wave1 = 0;
		if (count0 > song0[note0].len - 20) {
			if (count1 > song1[note1].len - 20) {
				DAC_Out(-1, -1);
			}
			else {
				DAC_Out(-1, wave1);
			}
		}
		else {
			if (count1 > song1[note1].len - 20) {
				DAC_Out(wave0, -1);
			}
			else {
				DAC_Out(wave0, wave1);
			}
		}
		if (count1 == song0[note0].len) {
			count1 = 0;
			note1++;
			if(note1 == 100){
				song_going = 0;
				disable_music();
				return;
			}
			TIMER1_TAILR_R = song1[note1].freq;
		}
	}
	wave1++;
	count1++;
}

void disable_music(){
	NVIC_DIS0_R = 1<<19;              // disable interrupt 19 in NVIC
	NVIC_DIS0_R = 1<<21;							// disable interrupt 21 in NVIC
	//SSI0_CR1_R = 0x00000000;        // disable SSI, master mode
}
