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
#include "TimerDriver.h"

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

#define C_2_8 20	
#define DF_1_8 21	
#define D_1_8 22	
#define EF_1_8 24	
#define E_1_8 25	
#define F_1_8 26	
#define GF_1_8 28	
#define G_1_8 30	
#define AF_1_8 31	
#define A_1_8 33	
#define BF_1_8 35	
#define B_1_8 37	
#define C_1_8 39	
#define DF0_8 42	
#define D0_8 44	
#define EF0_8 47	
#define E0_8 50	
#define F0_8 53	
#define GF0_8 56	
#define G0_8 59	
#define AF0_8 63	
#define A0_8 66	
#define BF0_8 70	
#define B0_8 74	
#define C0_8 79	
#define DF_8 83	
#define D_8 88	
#define EF_8 94	
#define E_8 99	
#define F_8 105	
#define GF_8 111	
#define G_8 118	
#define AF_8 125	
#define A_8 132	
#define BF_8 140	
#define B_8 148	
#define C_8 157	
#define DF1_8 167	
#define D1_8 176	
#define EF1_8 187	
#define E1_8 198	
#define F1_8 210	
#define GF1_8 222	
#define G1_8 235	
#define AF1_8 249	
#define A1_8 264	
#define BF1_8 280	
#define B1_8 297	
#define C1_8 314	
#define DF2_8 333	
#define D2_8 353	
#define EF2_8 374	
#define E2_8 396	
#define F2_8 419	
#define GF2_8 444	
#define G2_8 471	
#define AF2_8 499	
#define A2_8 528	
#define BF2_8 560	
#define B2_8 593	
#define C2_8 628	

#define C_2_16 10
#define DF_1_16 11
#define D_1_16 11
#define EF_1_16 12
#define E_1_16 12
#define F_1_16 13
#define GF_1_16 14
#define G_1_16 15
#define AF_1_16 16
#define A_1_16 17
#define BF_1_16 18
#define B_1_16 19
#define C_1_16 20
#define DF0_16 21
#define D0_16 22
#define EF0_16 23
#define E0_16 25
#define F0_16 26
#define GF0_16 28
#define G0_16 30
#define AF0_16 31
#define A0_16 33
#define BF0_16 35
#define B0_16 37
#define C0_16 39
#define DF_16 42
#define D_16 44
#define EF_16 47
#define E_16 50
#define F_16 53
#define GF_16 56
#define G_16 59
#define AF_16 62
#define A_16 66
#define BF_16 70
#define B_16 74
#define C_16 79
#define DF1_16 83
#define D1_16 88
#define EF1_16 93
#define E1_16 99
#define F1_16 105
#define GF1_16 111
#define G1_16 118
#define AF1_16 125
#define A1_16 132
#define BF1_16 140
#define B1_16 148
#define C1_16 157
#define DF2_16 166
#define D2_16 176
#define EF2_16 187
#define E2_16 198
#define F2_16 210
#define GF2_16 222
#define G2_16 235
#define AF2_16 249
#define A2_16 264
#define BF2_16 280
#define B2_16 296
#define C2_16 314

#define C_2_half 78		
#define DF_1_half 84		
#define D_1_half 88		
#define EF_1_half 94		
#define E_1_half 98		
#define F_1_half 104		
#define GF_1_half 110		
#define G_1_half 118		
#define AF_1_half 124		
#define A_1_half 132		
#define BF_1_half 140		
#define B_1_half 148		
#define C_1_half 156		
#define DF0_half 166		
#define D0_half 176		
#define EF0_half 186		
#define E0_half 198		
#define F0_half 210		
#define GF0_half 222		
#define G0_half 236		
#define AF0_half 250		
#define A0_half 264		
#define BF0_half 280		
#define B0_half 296		
#define C0_half 314		
#define DF_half 332		
#define D_half 352		
#define EF_half 374		
#define E_half 396		
#define F_half 420		
#define GF_half 444		
#define G_half 470		
#define AF_half 498		
#define A_half 528		
#define BF_half 560		
#define B_half 592		
#define C_half 628		
#define DF1_half 666		
#define D1_half 704		
#define EF1_half 746		
#define E1_half 792		
#define F1_half 838		
#define GF1_half 888		
#define G1_half 940		
#define AF1_half 996		
#define A1_half 1056		
#define BF1_half 1118		
#define B1_half 1186		
#define C1_half 1256		
#define DF2_half 1330		
#define D2_half 1410		
#define EF2_half 1494		
#define E2_half 1582		
#define F2_half 1676		
#define GF2_half 1776		
#define G2_half 1882		
#define AF2_half 1994		
#define A2_half 2112		
#define BF2_half 2238		
#define B2_half 2370		
#define C2_half 2512		

#define C_2_4 39		
#define DF_1_4 42		
#define D_1_4 44		
#define EF_1_4 47		
#define E_1_4 49		
#define F_1_4 52		
#define GF_1_4 55		
#define G_1_4 59		
#define AF_1_4 62		
#define A_1_4 66		
#define BF_1_4 70		
#define B_1_4 74		
#define C_1_4 78		
#define DF0_4 83		
#define D0_4 88		
#define EF0_4 93		
#define E0_4 99		
#define F0_4 105		
#define GF0_4 111		
#define G0_4 118		
#define AF0_4 125		
#define A0_4 132		
#define BF0_4 140		
#define B0_4 148		
#define C0_4 157		
#define DF_4 166		
#define D_4 176		
#define EF_4 187		
#define E_4 198		
#define F_4 210		
#define GF_4 222		
#define G_4 235		
#define AF_4 249		
#define A_4 264		
#define BF_4 280		
#define B_4 296		
#define C_4 314		
#define DF1_4 333		
#define D1_4 352		
#define EF1_4 373		
#define E1_4 396		
#define F1_4 419		
#define GF1_4 444		
#define G1_4 470		
#define AF1_4 498		
#define A1_4 528		
#define BF1_4 559		
#define B1_4 593		
#define C1_4 628		
#define DF2_4 665		
#define D2_4 705		
#define EF2_4 747		
#define E2_4 791		
#define F2_4 838		
#define GF2_4 888		
#define G2_4 941		
#define AF2_4 997		
#define A2_4 1056		
#define BF2_4 1119		
#define B2_4 1185		
#define C2_4 1256		

#define C_2_3 59		
#define DF_1_3 63		
#define D_1_3 66		
#define EF_1_3 71		
#define E_1_3 74		
#define F_1_3 78		
#define GF_1_3 83		
#define G_1_3 89		
#define AF_1_3 93		
#define A_1_3 99		
#define BF_1_3 105		
#define B_1_3 111		
#define C_1_3 117		
#define DF0_3 125		
#define D0_3 132		
#define EF0_3 140		
#define E0_3 149		
#define F0_3 158		
#define GF0_3 167		
#define G0_3 177		
#define AF0_3 188		
#define A0_3 198		
#define BF0_3 210		
#define B0_3 222		
#define C0_3 236		
#define DF_3 249		
#define D_3 264		
#define EF_3 281		
#define E_3 297		
#define F_3 315		
#define GF_3 333		
#define G_3 353		
#define AF_3 374		
#define A_3 396		
#define BF_3 420		
#define B_3 444		
#define C_3 471		
#define DF1_3 500		
#define D1_3 528		
#define EF1_3 560		
#define E1_3 594		
#define F1_3 629		
#define GF1_3 666		
#define G1_3 705		
#define AF1_3 747		
#define A1_3 792		
#define BF1_3 839		
#define B1_3 890		
#define C1_3 942		
#define DF2_3 998		
#define D2_3 1058		
#define EF2_3 1121		
#define E2_3 1187		
#define F2_3 1257		
#define GF2_3 1332		
#define G2_3 1412		
#define AF2_3 1496		
#define A2_3 1584		
#define BF2_3 1679		
#define B2_3 1778		
#define C2_3 1884		

#define C_2_whole 117		
#define DF_1_whole 126		
#define D_1_whole 132		
#define EF_1_whole 141		
#define E_1_whole 147		
#define F_1_whole 156		
#define GF_1_whole 165		
#define G_1_whole 177		
#define AF_1_whole 186		
#define A_1_whole 198		
#define BF_1_whole 210		
#define B_1_whole 222		
#define C_1_whole 234		
#define DF0_whole 249		
#define D0_whole 264		
#define EF0_whole 279		
#define E0_whole 297		
#define F0_whole 315		
#define GF0_whole 333		
#define G0_whole 354		
#define AF0_whole 375		
#define A0_whole 396		
#define BF0_whole 420		
#define B0_whole 444		
#define C0_whole 471		
#define DF_whole 498		
#define D_whole 528		
#define EF_whole 561		
#define E_whole 594		
#define F_whole 630		
#define GF_whole 666		
#define G_whole 705		
#define AF_whole 747		
#define A_whole 792		
#define BF_whole 840		
#define B_whole 888		
#define C_whole 942		
#define DF1_whole 999		
#define D1_whole 1056		
#define EF1_whole 1119		
#define E1_whole 1188		
#define F1_whole 1257		
#define GF1_whole 1332		
#define G1_whole 1410		
#define AF1_whole 1494		
#define A1_whole 1584		
#define BF1_whole 1677		
#define B1_whole 1779		
#define C1_whole 1884		
#define DF2_whole 1995		
#define D2_whole 2115		
#define EF2_whole 2241		
#define E2_whole 2373		
#define F2_whole 2514		
#define GF2_whole 2664		
#define G2_whole 2823		
#define AF2_whole 2991		
#define A2_whole 3168		
#define BF2_whole 3357		
#define B2_whole 3555		
#define C2_whole 3768		

#define C_2_6 30		
#define DF_1_6 32		
#define D_1_6 33		
#define EF_1_6 36		
#define E_1_6 38		
#define F_1_6 39		
#define GF_1_6 42		
#define G_1_6 45		
#define AF_1_6 47		
#define A_1_6 50		
#define BF_1_6 53		
#define B_1_6 56		
#define C_1_6 59		
#define DF0_6 63		
#define D0_6 66		
#define EF0_6 71		
#define E0_6 75		
#define F0_6 80		
#define GF0_6 84		
#define G0_6 89		
#define AF0_6 95		
#define A0_6 99		
#define BF0_6 105		
#define B0_6 111		
#define C0_6 119		
#define DF_6 125		
#define D_6 132		
#define EF_6 141		
#define E_6 149		
#define F_6 158		
#define GF_6 167		
#define G_6 177		
#define AF_6 188		
#define A_6 198		
#define BF_6 210		
#define B_6 222		
#define C_6 236		
#define DF1_6 251		
#define D1_6 264		
#define EF1_6 281		
#define E1_6 297		
#define F1_6 315		
#define GF1_6 333		
#define G1_6 353		
#define AF1_6 374		
#define A1_6 396		
#define BF1_6 420		
#define B1_6 446		
#define C1_6 471		
#define DF2_6 500		
#define D2_6 530		
#define EF2_6 561		
#define E2_6 594		
#define F2_6 629		
#define GF2_6 666		
#define G2_6 707		
#define AF2_6 749		
#define A2_6 792		
#define BF2_6 840		
#define B2_6 890		
#define C2_6 942		


#define guitar 0
#define trumpet 1
#define bassoon 2

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

typedef struct{
	int freq;
	int len;
	int pause;
} note;

const note song0[100] = {
	{G, G_3}, {C0, C0_3}, {C0, C0_8 }, {F, F_8 }, {G, G_4}, {C0, C0_4},
  {C0, C0_4}, {C0, C0_8}, {EF, EF_8}, {D, D_whole}, //{-1, }, //first 4 measures
	{F, F_3}, {BF0, BF0_3}, {BF0, BF0_8}, {EF, EF_8},
	{F, F_4}, {BF0, BF0_4}, {EF, EF_8}, {D, D_8},
	{C0, C0_whole}, // second 4 measures
	{G, G_3}, {C0, C0_3}, {C0, C0_8}, {F, F_8},
	{G, G_4}, {C0, C0_4}, {C0, C0_8}, {EF, EF_8},
	{D, D_whole}, // third 4 measures
	{F,F_3}, {BF0, BF0_3}, {D, D_6}, {EF, EF_6}, {D, D_6}, 
	{BF0, BF0_6}, {C0, C0_whole}, //fourth 4 measures
	{C, C_whole}, {BF, BF_whole}, {C0, C0_whole},
	{G, G_whole},//fifth 4 measures
	{EF, EF_whole}, {D, D_3}, {F, F_3}, {G, G_whole*2}
};
const note song1[177] = {
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{G_1, G_1_8, 0}, {BF_1, BF_1_16, 0}, {C_1, C_1_16, 0}, {D0, D0_8, 0},
	{G_1, G_1_8, 0}, {BF_1, BF_1_16, 0}, {C_1, C_1_16, 0}, {D0, D0_8, 0},
	{G_1, G_1_8, 0}, {BF_1, BF_1_16, 0}, {C_1, C_1_16, 0}, {D0, D0_8, 0},
	{G_1, G_1_8, 0}, {BF_1, BF_1_16, 0}, {C_1, C_1_16, 0}, {D0, D0_8, 0},
	{BF_1, BF_1_8, 0}, {D0, D0_16, 0}, {EF0, EF0_16, 0}, {F0, F0_8, 0},
	{BF_1, BF_1_8, 0}, {D0, D0_16, 0}, {EF0, EF0_16, 0}, {F0, F0_8, 0},
	{BF_1, BF_1_8, 0}, {D0, D0_16, 0}, {EF0, EF0_16, 0}, {F0, F0_8, 0},
	{BF_1, BF_1_8, 0}, {D0, D0_16, 0}, {EF0, EF0_16, 0}, {F0, F0_8, 0},
	{F_1, F_1_8, 0}, {AF_1, AF_1_16, 0}, {BF_1, BF_1_16, 0}, {C_1, C_1_8, 0},
	{F_1, F_1_8, 0}, {AF_1, AF_1_16, 0}, {BF_1, BF_1_16, 0}, {C_1, C_1_8, 0},
	{F_1, F_1_8, 0}, {AF_1, AF_1_16, 0}, {BF_1, BF_1_16, 0}, {C_1, C_1_8, 0},
	{F_1, F_1_8, 0}, {AF_1, AF_1_16, 0}, {BF_1, BF_1_16, 0}, {C_1, C_1_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{G_1, G_1_8, 0}, {BF_1, BF_1_16, 0}, {C_1, C_1_16, 0}, {D0, D0_8, 0},
	{G_1, G_1_8, 0}, {BF_1, BF_1_16, 0}, {C_1, C_1_16, 0}, {D0, D0_8, 0},
	{G_1, G_1_8, 0}, {BF_1, BF_1_16, 0}, {C_1, C_1_16, 0}, {D0, D0_8, 0},
	{G_1, G_1_8, 0}, {BF_1, BF_1_16, 0}, {C_1, C_1_16, 0}, {D0, D0_8, 0},
	{BF_1, BF_1_8, 0}, {D0, D0_16, 0}, {EF0, EF0_16, 0}, {F0, F0_8, 0},
	{BF_1, BF_1_8, 0}, {D0, D0_16, 0}, {EF0, EF0_16, 0}, {F0, F0_8, 0},
	{BF_1, BF_1_8, 0}, {D0, D0_16, 0}, {EF0, EF0_16, 0}, {F0, F0_8, 0},
	{BF_1, BF_1_8, 0}, {D0, D0_16, 0}, {EF0, EF0_16, 0}, {F0, F0_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{AF0, AF0_8, 0}, {C0, C0_16, 0}, {D, D_16, 0}, {EF, EF_8, 0},
	{AF0, AF0_8, 0}, {C0, C0_16, 0}, {D, D_16, 0}, {EF, EF_8, 0},
	{EF0, EF0_8, 0}, {G0, G0_16, 0}, {AF0, AF0_16, 0}, {BF0, BF0_8, 0},
	{EF0, EF0_8, 0}, {G0, G0_16, 0}, {C0, C0_16, 0}, {BF0, BF0_8, 0},
	{F0, F0_8, 0}, {AF0, AF0_16, 0}, {BF0, BF0_16, 0}, {C0, C0_8, 0},
	{F0, F0_8, 0}, {AF0, AF0_16, 0}, {BF0, BF0_16, 0}, {C0, C0_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {F0, F0_16, 0}, {G0, G0_8, 0},
	{C_1, C_1_8, 0}, {EF0, EF0_16, 0}, {AF0, AF0_16, 0}, {G0, G0_8, 0},
	{AF0, AF0_8, 0}, {C0, C0_16, 0}, {D, D_16, 0}, {EF, EF_8, 0},
	{AF0, AF0_8, 0}, {C0, C0_16, 0}, {D, D_16, 0}, {EF, EF_8, 0},
	{BF_1, BF_1_8, 0}, {D0, D0_16, 0}, {EF0, EF0_16, 0}, {F0, F0_8, 0},
	{BF_1, BF_1_8, 0}, {D0, D0_16, 0}, {EF0, EF0_16, 0}, {F0, F0_8, 0},
	{C_1, C_1_whole, 0}
};

uint32_t wave0, wave1, count0, count1, note0, note1, instrument;
char on_button, song_going;	
int main(void){
  uint32_t i=0;
  SysTick_Init();
	while(1){
		if(on_button == 1){
			song_going = 1;
			wave0 = 0;
			wave1 = 0;
			note0 = 0;
			note1 = 0;
			count0 = 0;
			count1 = 0;
			instrument = 0;
			DisableInterrupts();
			Timer0A_Init();
			Timer1A_Init();
			DAC_Init();
			EnableInterrupts();
			while(song_going == 1){
				
			}
		}
	}
}
