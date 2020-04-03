#ifndef __MAIN_H
#define __MAIN_H
#include "includes.h"


extern OS_EVENT *FramMutex;
#define CARDTASKSTKLEN			256		//卡任务
#define ELMTASKSTKLEN		  	256		//电表任务
#define NETTASKSTKLEN		  	512	//网络任务
#define CHGTASKSTKLEN  			256   //充电任务
#define MAINTASKSTKLEN		  512	//网络任务
#define PROTASKSTKLEN		  	512	//网络任务



#define MUTEX_FRAMPRIO			3

#define CHGTASKPRIO				  10		//充电任务
#define TCPIP_THREAD_PRIO		11  	//定义内核任务的优先级为5
#define PROTASKPRIO					12		//通讯协议解析任务
#define NETTASKPRIO				  13		//网络任务
#define MAINTASKPRIO				14		//主任务
#define CARDTASKPRIO				15		//卡任务
#define ELMTASKPRIO				  16		//电表





 

//LED任务
//任务优先级
#define LED_TASK_PRIO		10
//任务堆栈大小
#define LED_STK_SIZE		128
//任务堆栈

//任务函数


//在LCD上显示地址信息任务
//任务优先级
#define DISPLAY_TASK_PRIO	11
//任务堆栈大小
#define DISPLAY_STK_SIZE	128
//任务堆栈

//任务函数
void display_task(void *pdata);
void led_task(void *pdata);




#endif
