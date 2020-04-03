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


#define FRAMADDR_UPDATAINFO    0   //���ò����洢��ַ
//����
typedef struct{
	uint16_t FLAG; //�汾�� 0x5A5A5A02	
	uint32_t applen;
}UPDATAINFO;   //�ṹ���С���ܳ���8�ֽ�
#define UPDATAFLAG     0x5A5A

uint32_t GetApplenth(uint8_t flag);

void FramInit(void);
void FramWrite(u16 addr,u8 *data,u16 num);
void FramRead(u16 addr,u8 *data,u16 num);
void SetApplenth(uint16_t flag, uint32_t len);
#endif













