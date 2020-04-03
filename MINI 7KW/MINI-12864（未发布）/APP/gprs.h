#ifndef __GPRS_H
#define __GPRS_H
#include "sys.h"

#define GPRS_RET_H()   PDout(4)  = 1            //复位管脚
#define GPRS_RET_L() 	 PDout(4)  = 0  

#define ME4GDATAMAXLEN            1024
#define GPRSUART                  2
typedef struct{
	uint8_t ReBuf[ME4GDATAMAXLEN];
	uint16_t RecvWr;
	uint16_t RecvRd;
	uint8_t  Signal;
}GPRS4GINFO;

//外部调用函数
uint8_t GetSignal(void);                                           //获取信号强度函数
uint8_t GprsCon(uint8_t sn, uint8_t serverip[4], uint16_t port);   //连接服务器函数
uint16_t GprsRecv(uint8_t sn, uint8_t *buffer, uint16_t len);      //接收函数
uint8_t GprsSend(uint8_t sn, uint8_t *buffer, uint16_t len);       //发送函数
void GprsTask(void* pdata);
void GPRSInit(void);
void GprsProc(void);
#endif




