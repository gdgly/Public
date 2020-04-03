#include "sys.h"
#include "delay.h"  
#include "rtc.h" 		    
#include "stm32f4xx.h"


/************************************************************************************************************
** �� �� �� : RTCInit
** �������� : RTCʵʱʱ�ӳ�ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	SUCEE(1),�ɹ�   ERROR(0),�����ʼ��ģʽʧ�� 
*************************************************************************************************************
*/
uint8_t RTCInit(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	u16 retry=0X1FFF; 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);//ʹ��PWRʱ��
	PWR_BackupAccessCmd(ENABLE);	//ʹ�ܺ󱸼Ĵ������� 
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0)!=0x5050)		//�Ƿ��һ������?
	{
		RCC_LSEConfig(RCC_LSE_ON);//LSE ����    
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//���ָ����RCC��־λ�������,�ȴ����پ������
			{
			retry++;
			delay_ms(10);
			}
		if(retry==0)return 1;		//LSE ����ʧ��. 
			
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
		RCC_RTCCLKCmd(ENABLE);	//ʹ��RTCʱ�� 

    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;//RTC�첽��Ƶϵ��(1~0X7F)
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;//RTCͬ����Ƶϵ��(0~7FFF)
    RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;//RTC����Ϊ,24Сʱ��ʽ
    RTC_Init(&RTC_InitStructure);
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//����Ѿ���ʼ������
	} 
 
	return 0;
}		 				    



/************************************************************************************************************
** �� �� �� : RTC_Set
** �������� : RTCʵʱʱ������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	SUCEE(1),�ɹ�   ERROR(0),�����ʼ��ģʽʧ�� 
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
** �� �� �� : RtcRead
** �������� : RTCʵʱʱ�Ӷ�ȡ
** ��    �� : rtc
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : RtcWrite
** �������� : RTCʵʱʱ������
** ��    �� : rtc
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void RtcWrite(Rtc *rtc)
{
	RTC_Set(rtc->year,rtc->month,rtc->day, rtc->hour,rtc->min,rtc->sec);
}













