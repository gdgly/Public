#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define BUFFER_LEN	       768   
#define UART_MAX   4
#define DEV_UART1  0
#define DEV_UART2  1
#define DEV_UART4  2
#define DEV_UART5  3
typedef	struct 
{
	uint16_t Wr;
	uint16_t Rd;
	uint8_t  buf[BUFFER_LEN];
}UARTBUF;

void Uart1Init(u32 bound);
void Uart2Init(u32 bound);
void Uart4Init(u32 bound);
void Uart5Init(u32 bound);
void UartInit(uint8_t no, uint32_t bound);
uint16_t UartWrite( uint8_t no, uint8_t *data, uint16_t datalen);
uint16_t UartRead(uint8_t no,uint8_t *data, uint16_t datalen);
void UartFlush( uint8_t no);
void printfx(uint8_t *data,uint16_t datalen);
#endif


