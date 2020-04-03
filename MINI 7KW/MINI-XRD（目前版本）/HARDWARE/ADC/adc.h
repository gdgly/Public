#ifndef __ADC_H
#define __ADC_H	 
#include "sys.h"

#define		ADCVALUESIZE   50
#define		ADC12V		4
#define		ADC9V			3
#define		ADC6V			2
#define 	ADC0V			1

#define ADTRANSITION		75 
#define MIN6V	 				5000		//��Ӧ5V
#define MAX6V					7000		//��Ӧ7V
#define MIN9V	 				8000		//��Ӧ8V
#define MAX9V					10000 		//��Ӧ10V
#define MIN12V				10260		//�Ĵ���ȡֵ2717	  ��Ӧδ��ǹ����Сȡֵ��Χ
#define MAX12V				12092		//�Ĵ���ȡֵ3211		��Ӧδ��ǹ�����ȡֵ��Χ



void ADCInit(void);
void ADCScan(void);
uint32_t GetCpVol(void);	 	
uint8_t GetAdcStatus(void);
#endif


