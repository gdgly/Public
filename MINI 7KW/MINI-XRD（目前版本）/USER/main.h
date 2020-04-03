#ifndef __MAIN_H
#define __MAIN_H
#include "includes.h"


extern OS_EVENT *FramMutex;
#define CARDTASKSTKLEN			256		//������
#define ELMTASKSTKLEN		  	256		//�������
#define NETTASKSTKLEN		  	512	//��������
#define CHGTASKSTKLEN  			256   //�������
#define MAINTASKSTKLEN		  512	//��������
#define PROTASKSTKLEN		  	512	//��������



#define MUTEX_FRAMPRIO			3

#define CHGTASKPRIO				  10		//�������
#define TCPIP_THREAD_PRIO		11  	//�����ں���������ȼ�Ϊ5
#define PROTASKPRIO					12		//ͨѶЭ���������
#define NETTASKPRIO				  13		//��������
#define MAINTASKPRIO				14		//������
#define CARDTASKPRIO				15		//������
#define ELMTASKPRIO				  16		//���





 

//LED����
//�������ȼ�
#define LED_TASK_PRIO		10
//�����ջ��С
#define LED_STK_SIZE		128
//�����ջ

//������


//��LCD����ʾ��ַ��Ϣ����
//�������ȼ�
#define DISPLAY_TASK_PRIO	11
//�����ջ��С
#define DISPLAY_STK_SIZE	128
//�����ջ

//������
void display_task(void *pdata);
void led_task(void *pdata);




#endif
