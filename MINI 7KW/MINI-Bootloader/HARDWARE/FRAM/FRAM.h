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


#define FRAMADDR_UPDATAINFO    0   //配置参数存储地址
//参数
typedef struct{
	uint16_t FLAG; //版本号 0x5A5A5A02	
	uint32_t applen;
}UPDATAINFO;   //结构体大小不能超过8字节
#define UPDATAFLAG     0x5A5A

uint32_t GetApplenth(uint8_t flag);

void FramInit(void);
void FramWrite(u16 addr,u8 *data,u16 num);
void FramRead(u16 addr,u8 *data,u16 num);
void SetApplenth(uint16_t flag, uint32_t len);
#endif













