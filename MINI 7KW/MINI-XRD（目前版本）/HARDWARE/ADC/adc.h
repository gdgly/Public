#ifndef __ADC_H
#define __ADC_H	 
#include "sys.h"

#define		ADCVALUESIZE   50
#define		ADC12V		4
#define		ADC9V			3
#define		ADC6V			2
#define 	ADC0V			1

#define ADTRANSITION		75 
#define MIN6V	 				5000		//对应5V
#define MAX6V					7000		//对应7V
#define MIN9V	 				8000		//对应8V
#define MAX9V					10000 		//对应10V
#define MIN12V				10260		//寄存器取值2717	  对应未插枪的最小取值范围
#define MAX12V				12092		//寄存器取值3211		对应未插枪的最大取值范围



void ADCInit(void);
void ADCScan(void);
uint32_t GetCpVol(void);	 	
uint8_t GetAdcStatus(void);
#endif


