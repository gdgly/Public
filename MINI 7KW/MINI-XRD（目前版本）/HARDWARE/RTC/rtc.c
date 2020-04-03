#include "sys.h"
#include "delay.h"  
#include "rtc.h" 		    
#include "stm32f4xx.h"


/************************************************************************************************************
** 函 数 名 : RTCInit
** 功能描述 : RTC实时时钟初始化
** 输    入 : 无
** 输    出 : 无
** 返    回 :	SUCEE(1),成功   ERROR(0),进入初始化模式失败 
*************************************************************************************************************
*/
uint8_t RTCInit(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry=0X1FFF; 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//使能PWR时钟
	PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问 
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//是否第一次配置?
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE 开启    
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//检查指定的RCC标志位设置与否,等待低速晶振就绪
			{
			retry++;
			delay_ms(10);
			}
		if(retry==0)return 1;		//LSE 开启失败. 
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//设置RTC时钟(RTCCLK),选择LSE作为RTC时钟    
		RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟 

    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC异步分频系数(1~0X7F)
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTC同步分频系数(0~7FFF)
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC设置为,24小时格式
    RTC_Init(&RTC_InitStructure);
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//标记已经初始化过了
	} 
 
	return 0;
}		 				    



/************************************************************************************************************
** 函 数 名 : RTC_Set
** 功能描述 : RTC实时时钟设置
** 输    入 : 无
** 输    出 : 无
** 返    回 :	SUCEE(1),成功   ERROR(0),进入初始化模式失败 
*************************************************************************************************************
*/
uint8_t RTC_Set(u16 syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec)
{

	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	
	RTC_DateTypeInitStructure.RTC_Date = sday;
	RTC_DateTypeInitStructure.RTC_Month = smon;
	RTC_DateTypeInitStructure.RTC_WeekDay = 1;
	RTC_DateTypeInitStructure.RTC_Year=(syear %100);

	
	RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
	RTC_WaitForSynchro();
		
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12 = RTC_H12_AM;
	
  RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);

	RTC_WaitForSynchro();
	return 0;	    
}

/************************************************************************************************************
** 函 数 名 : RtcRead
** 功能描述 : RTC实时时钟读取
** 输    入 : rtc
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void RtcRead(Rtc *rtc)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	RTC_WaitForSynchro();
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	
	
	rtc->hour = RTC_TimeStruct.RTC_Hours;
	rtc->min = RTC_TimeStruct.RTC_Minutes;
	rtc->sec = RTC_TimeStruct.RTC_Seconds;
	
	RTC_WaitForSynchro();
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	rtc->year = RTC_DateStruct.RTC_Year + 2000;
	rtc->month = RTC_DateStruct.RTC_Month;
	rtc->day = RTC_DateStruct.RTC_Date;
}	 
																					 

/************************************************************************************************************
** 函 数 名 : RtcWrite
** 功能描述 : RTC实时时钟设置
** 输    入 : rtc
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void RtcWrite(Rtc *rtc)
{
	RTC_Set(rtc->year,rtc->month,rtc->day, rtc->hour,rtc->min,rtc->sec);
}













