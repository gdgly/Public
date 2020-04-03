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


u8 RTCInit(void);//��ʼ��RTC,����0,ʧ��;1,�ɹ�;

void RtcRead(Rtc *rtc); //��ȡʱ��   
void RtcWrite(Rtc *rtc);//����ʱ��   

u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//����ʱ��			 
#endif


