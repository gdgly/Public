#ifndef _LED_H
#define _LED_H
#include "sys.h"


#define LEDRed PBout(8)	// PB8
#define LEDYellow PBout(9)	// PB9
#define LEDGreen PEout(0)	// PE0
#define LEDRun PCout(8)	// PC8

void LED_Init(void);//≥ı ºªØ
#endif
