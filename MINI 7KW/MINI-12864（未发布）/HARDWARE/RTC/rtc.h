#ifndef __RTC_H
#define __RTC_H	    
#include "sys.h" 

//时间结构体
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	vu8  day;
	vu8  month;
	vu16 year;		 
}Rtc;					 


u8 RTCInit(void);//初始化RTC,返回0,失败;1,成功;

void RtcRead(Rtc *rtc); //获取时间   
void RtcWrite(Rtc *rtc);//设置时间   

u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//设置时间			 
#endif


