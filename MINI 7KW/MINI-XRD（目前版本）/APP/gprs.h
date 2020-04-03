#ifndef __GPRS_H
#define __GPRS_H
#include "sys.h"

#define GPRS_RET_H()   PDout(4)  = 1            //��λ�ܽ�
#define GPRS_RET_L() 	 PDout(4)  = 0  

#define ME4GDATAMAXLEN            1024
#define GPRSUART                  2
typedef struct{
	uint8_t ReBuf[ME4GDATAMAXLEN];
	uint16_t RecvWr;
	uint16_t RecvRd;
	uint8_t  Signal;
}GPRS4GINFO;

//�ⲿ���ú���
uint8_t GetSignal(void);                                           //��ȡ�ź�ǿ�Ⱥ���
uint8_t GprsCon(uint8_t sn, uint8_t serverip[4], uint16_t port);   //���ӷ���������
uint16_t GprsRecv(uint8_t sn, uint8_t *buffer, uint16_t len);      //���պ���
uint8_t GprsSend(uint8_t sn, uint8_t *buffer, uint16_t len);       //���ͺ���
void GprsTask(void* pdata);
void GPRSInit(void);
void GprsProc(void);
#endif




