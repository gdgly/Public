#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "includes.h"

#include "MainTask.h"
#include "rtc.h"
#include "Para.h"
#include "pwm.h"
#include "main.h"
#include "CardTask.h"
#include "Chgtask.h"
#include "adc.h"
#include "Display.h"
#include "ElmTask.h"
#include "QR_Encode.h"
#include "Chgtask.h"
#include "NetTask.h"
#include "Record.h"
#include "Protocol.h"
#include "lcd_12864.h"  
#include "FRAM.h"


static OS_STK CardTaskStk[CARDTASKSTKLEN];//卡任务
static OS_STK ElmTaskStk[ELMTASKSTKLEN];	//电表任务
static OS_STK NetTaskStk[NETTASKSTKLEN];	//电表任务
static OS_STK ChgTaskStk[CHGTASKSTKLEN];	//电表任务
static OS_STK MainTaskStk[MAINTASKSTKLEN];	//主任务
static OS_STK ProTaskStk[PROTASKSTKLEN];	//主任务

OS_EVENT *FramMutex;


void BoardInit()
{
	
	delay_init(168);       	//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断分组配置
	Uart1Init(115200);
	LED_Init();
	RTCInit();
	LCD_Init();
	InitChgGpio();
	TIMInit(9999, 83); //10ms中断1一次
	PwmInit();
	ADCInit();
	FramInit();
}



int main(void)
{
	uint8_t os_err;
	SCB->VTOR = FLASH_BASE | 0x4000; /* Vector Table Relocation in Internal FLASH. */
	BoardInit();
	ParaInit();//参数初始化
	InitQrCodeData();
	RecordInit();
	OSInit();				//UCOS初始化
	INTX_DISABLE();  	//关中断
	
	
	FramMutex = OSMutexCreate(MUTEX_FRAMPRIO, &os_err);
	OSTaskCreate(NetTask, (void*)0, (OS_STK*)&NetTaskStk[NETTASKSTKLEN-1],  NETTASKPRIO); 		//创建网络任务
	OSTaskCreate(CardTask, (void*)0, (OS_STK*)&CardTaskStk[CARDTASKSTKLEN-1], CARDTASKPRIO); 	//创建刷卡任务
	OSTaskCreate(ElmTask, (void*)0, (OS_STK*)&ElmTaskStk[ELMTASKSTKLEN-1], ELMTASKPRIO); 			//创建电表任务
	OSTaskCreate(ChargeTask, (void*)0, (OS_STK*)&ChgTaskStk[CHGTASKSTKLEN-1], CHGTASKPRIO); 		//创建充电任务
	OSTaskCreate(MainTask, (void*)0, (OS_STK*)&MainTaskStk[MAINTASKSTKLEN-1], MAINTASKPRIO); 		//创建充任务
	OSTaskCreate(TaskProtocolHandle, (void*)0, (OS_STK*)&ProTaskStk[PROTASKSTKLEN-1], PROTASKPRIO); 		//创建充任务
	INTX_ENABLE();  //开中断
	OSStart(); //开启UCOS
}








