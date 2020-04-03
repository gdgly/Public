#include "stmflash.h"
#include "delay.h"
#include "usart.h" 


 
//��ȡָ����ַ�İ���(16λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}  
//��ȡĳ����ַ���ڵ�flash����
//addr:flash��ַ
//����ֵ:0~11,��addr���ڵ�����
uint16_t STMFLASH_GetFlashSector(u32 addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}




/************************************************************************************************************
** �� �� �� : STMFLASH_ReadAndWrite
** �������� : ��FLASH  ReadAddr ������д�� FLASH WriteAddr
** ��    �� : ReadAddr:����ַ   WriteAddr:д��ַ   len: ����  �ֽ�
** ��    �� : ��
** ��    �� :	Applenth
*************************************************************************************************************
*/
void STMFLASH_ReadAndWrite(u32 ReadAddr,u32 WriteAddr,u32 len)
{
	u16 i;
	uint32_t Temp;
	if((len % 4 ) == 0)
		len = len / 4; 
	else
		len = (len / 4) + 1;
	
	FLASH_Unlock();									//���� 
  FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���
	for(i=0;i<len;i++){
		Temp = STMFLASH_ReadWord(ReadAddr);
		FLASH_ProgramWord(WriteAddr,Temp);
	  WriteAddr+=4;//��ַ����4.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
	FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����
}	

/************************************************************************************************************
** �� �� �� : EraseAPP1
** �������� : �������г�������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void EraseAPP1(void)
{
	FLASH_Unlock();									//���� 
  FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���
	
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_1),VoltageRange_3);
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_2),VoltageRange_3);
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_3),VoltageRange_3);
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_4),VoltageRange_3);
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_5),VoltageRange_3);
	
	FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����
}

/************************************************************************************************************
** �� �� �� : ReadAppAndWrite
** �������� : 
** ��    �� : ReadAddr:����ַ   WriteAddr:д��ַ   len: ����
** ��    �� : ��
** ��    �� :	Applenth
*************************************************************************************************************
*/
void ReadApp2ToApp1(u32 App2addr,u32 App1addr, u32 len)	
{
	if(len == 0)
		return;
	printf("Erase code\r\n");  
	EraseAPP1();
	printf("Write code\r\n"); 
	STMFLASH_ReadAndWrite(App2addr, App1addr, len);
	printf("Write code End\r\n"); 


}











