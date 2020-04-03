#ifndef __RTC_H
#define __RTC_H	    
#include "sys.h" 

//ʱ��ṹ��
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	vu8  day;
	vu8  month;
	vu16 year;		 
}Rtc;					 


uint8_t RTCInit(void);//��ʼ��RTC,����0,ʧ��;1,�ɹ�;

void RtcRead(Rtc *rtc); //��ȡʱ��   
void RtcWrite(Rtc *rtc);//����ʱ��   

uint8_t RTC_Set(u16 syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec);//����ʱ��			 
#endif


