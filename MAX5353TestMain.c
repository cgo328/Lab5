// MAX5353TestMain.c
// Runs on LM4F120/TM4C123
// Test the functions provided in MAX5353.c by outputting
// a sine wave at a particular frequency.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
#include "MAX5353.h"
#include "SysTick.h"

// 12-bit 32-element sine wave
// multiply each value by 2 to shift into bits 12:1 of SSI packet
// three control bits in 15:13 are all zero for immediate DAC update
// book figure shows MAX5353 in unipolar rail-to-rail configuration
// that means when wave[n] = 0x0000 (LSB = 0), output = 0
//                 wave[n] = 0x1000 (LSB = 0), output = Vref
//                 wave[n] = 0x1FFE (LSB = 0), output = 2*Vref
const uint16_t wave[32] = {
  2048*2,2448*2,2832*2,3186*2,3496*2,3751*2,3940*2,4057*2,4095*2,4057*2,3940*2,
  3751*2,3496*2,3186*2,2832*2,2448*2,2048*2,1648*2,1264*2,910*2,600*2,345*2,
  156*2,39*2,0*2,39*2,156*2,345*2,600*2,910*2,1264*2,1648*2};

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

typedef struct{
	int freq;
	int len;
} note;

const note song0[100] = {
	{G, 353},
	{C0, 235}
};
const note song1[100];

uint32_t wave0, wave1, count0, count1, note0, note1, instrument;
	
int main(void){
  uint32_t i=0;
  DAC_Init(0x1000);                  // initialize with command: Vout = Vref
  SysTick_Init();
  while(1){
    DAC_Out(wave[i&0x1F]);
    i = i + 1;
    // calculated frequencies are not exact, due to the impreciseness of delay loops
    // assumes using 16 MHz PIOSC (default setting for clock source)
    // maximum frequency with 16 MHz PIOSC: (8,000,000 bits/1 sec)*(1 sample/16 bits)*(1 wave/32 sample) = 15,625 Hz
    // maximum frequency with 20 MHz PLL: (10,000,000 bits/1 sec)*(1 sample/16 bits)*(1 wave/32 sample) = 19,531.25 Hz
//    SysTick_Wait(0);                 // ?? kHz sine wave (actually 12,000 Hz)
//    SysTick_Wait(9);                 // 55.6 kHz sine wave (actually 10,000 Hz)
//    SysTick_Wait(15);                // 33.3 kHz sine wave (actually 8,500 Hz)
//    SysTick_Wait(19);                // 26.3 kHz sine wave (actually 8,500 Hz)
//    SysTick_Wait(64);                // 7.81 kHz sine wave (actually 4,800 Hz)
//    SysTick_Wait(99);                // 5.05 kHz sine wave (actually 3,500 Hz)
    SysTick_Wait(1136);              // 440 Hz sine wave (actually 420 Hz)
//    SysTick_Wait(50000);             // 10 Hz sine wave (actually 9.9 Hz)
  }
}
