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


static OS_STK CardTaskStk[CARDTASKSTKLEN];//������
static OS_STK ElmTaskStk[ELMTASKSTKLEN];	//�������
static OS_STK NetTaskStk[NETTASKSTKLEN];	//�������
static OS_STK ChgTaskStk[CHGTASKSTKLEN];	//�������
static OS_STK MainTaskStk[MAINTASKSTKLEN];	//������
static OS_STK ProTaskStk[PROTASKSTKLEN];	//������

OS_EVENT *FramMutex;


void BoardInit()
{
	
	delay_init(168);       	//��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϷ�������
	Uart1Init(115200);
	LED_Init();
	RTCInit();
	LCD_Init();
	InitChgGpio();
	TIMInit(9999, 83); //10ms�ж�1һ��
	PwmInit();
	ADCInit();
	FramInit();
}



int main(void)
{
	uint8_t os_err;
	SCB->VTOR = FLASH_BASE | 0x4000; /* Vector Table Relocation in Internal FLASH. */
	BoardInit();
	ParaInit();//������ʼ��
	InitQrCodeData();
	RecordInit();
	OSInit();				//UCOS��ʼ��
	INTX_DISABLE();  	//���ж�
	
	
	FramMutex = OSMutexCreate(MUTEX_FRAMPRIO, &os_err);
	OSTaskCreate(NetTask, (void*)0, (OS_STK*)&NetTaskStk[NETTASKSTKLEN-1],  NETTASKPRIO); 		//������������
	OSTaskCreate(CardTask, (void*)0, (OS_STK*)&CardTaskStk[CARDTASKSTKLEN-1], CARDTASKPRIO); 	//����ˢ������
	OSTaskCreate(ElmTask, (void*)0, (OS_STK*)&ElmTaskStk[ELMTASKSTKLEN-1], ELMTASKPRIO); 			//�����������
	OSTaskCreate(ChargeTask, (void*)0, (OS_STK*)&ChgTaskStk[CHGTASKSTKLEN-1], CHGTASKPRIO); 		//�����������
	OSTaskCreate(MainTask, (void*)0, (OS_STK*)&MainTaskStk[MAINTASKSTKLEN-1], MAINTASKPRIO); 		//����������
	OSTaskCreate(TaskProtocolHandle, (void*)0, (OS_STK*)&ProTaskStk[PROTASKSTKLEN-1], PROTASKPRIO); 		//����������
	INTX_ENABLE();  //���ж�
	OSStart(); //����UCOS
}








