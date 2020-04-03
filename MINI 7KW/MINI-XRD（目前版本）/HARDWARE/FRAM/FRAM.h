#ifndef _FRAM_H
#define _FRAM_H                        
#include "sys.h" 


#define        FM25CL64_CS                 PAout(15)                  //FM25CL64Ƭѡ

////FM25CL64ָ���
#define	FM_WREN		0X06	//дʹ��
#define	FM_WRDI		0X04	//д��ֹ
#define	FM_RDSR		0X05	//��״̬�Ĵ���
#define	FM_WRSR		0X01	//д״̬�Ĵ���
#define	FM_READ		0X03	//������
#define	FM_WRITE	0X02	//д����


void FramInit(void);
void FramWrite(u16 addr,uint8_t *data,u16 num);
void FramRead(u16 addr,uint8_t *data,u16 num);
#endif













