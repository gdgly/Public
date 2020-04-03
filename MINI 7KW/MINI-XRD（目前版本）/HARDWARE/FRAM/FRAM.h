#ifndef _FRAM_H
#define _FRAM_H                        
#include "sys.h" 


#define        FM25CL64_CS                 PAout(15)                  //FM25CL64片选

////FM25CL64指令定义
#define	FM_WREN		0X06	//写使能
#define	FM_WRDI		0X04	//写禁止
#define	FM_RDSR		0X05	//读状态寄存器
#define	FM_WRSR		0X01	//写状态寄存器
#define	FM_READ		0X03	//读数据
#define	FM_WRITE	0X02	//写数据


void FramInit(void);
void FramWrite(u16 addr,uint8_t *data,u16 num);
void FramRead(u16 addr,uint8_t *data,u16 num);
#endif













