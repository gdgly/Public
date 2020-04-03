#include "stmflash.h"
#include "delay.h"
#include "usart.h" 
#include "sys.h" 


 
//��ȡָ����ַ�İ���(32λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
static u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}  
//��ȡĳ����ַ���ڵ�flash����
//addr:flash��ַ
//����ֵ:0~11,��addr���ڵ�����
static uint16_t STMFLASH_GetFlashSector(u32 addr)
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
static void STMFLASH_ReadAndWrite(u32 ReadAddr,u32 WriteAddr,u32 len)
{
	u16 i;
	uint32_t Temp;
	
	if((len % 4 ) == 0)
		len = len / 4; 
	else
		len = (len / 4) + 1;
	INTX_DISABLE();
	for(i=0;i<len;i++){
		Temp = STMFLASH_ReadWord(ReadAddr);
		FLASH_ProgramWord(WriteAddr,Temp);
	  WriteAddr+=4;//��ַ����4.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
	INTX_ENABLE();
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
	INTX_DISABLE();
	FLASH_Unlock();									//���� 
  FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���
	
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_1),VoltageRange_3);
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_2),VoltageRange_3);
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_3),VoltageRange_3);
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_4),VoltageRange_3);
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_5),VoltageRange_3);
	
	FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����
	INTX_ENABLE();
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
	EraseAPP1();
	STMFLASH_ReadAndWrite(App2addr, App1addr, len);


}




/************************************************************************************************************
** �� �� �� : procress_erase
** �������� : ����APP2 ����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	1 ʧ�� 0 �ɹ�
*************************************************************************************************************
*/
uint8_t procress_erase(void)
{
	
	INTX_DISABLE();
	FLASH_Unlock();									//���� 
	FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���
	if(FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_6),VoltageRange_3)!=FLASH_COMPLETE)//д������
	{ 
			FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
			FLASH_Lock();//����
			INTX_ENABLE();
			return  1;
	}	
	if(FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_7),VoltageRange_3)!=FLASH_COMPLETE)//д������
	{ 
			FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
			FLASH_Lock();//����
			INTX_ENABLE();
			return 1;
	}	

	
	FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����
	INTX_ENABLE();
	return 0;
}


void FLASH_Write(uint32_t WriteAddr, uint8_t *pBuffer,u32 NumToWrite)
{	
	u32 i;
	if((NumToWrite % 4 ) == 0)
		NumToWrite = NumToWrite / 4; 
	else
		NumToWrite = (NumToWrite / 4) + 1;
	
	INTX_DISABLE();
	FLASH_Unlock();									//���� 
	FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���
	for(i=0;i<NumToWrite;i++){
		FLASH_ProgramWord(WriteAddr, *(uint32_t *)pBuffer);
	  WriteAddr += 4;//��ַ����4.
		pBuffer += 4;//ƫ��4���ֽ�.	
	}
	FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����
	INTX_ENABLE();
	
}


/************************************************************************************************************
** �� �� �� : process_update
** �������� : IAP ����д��
** ��    �� : Index �����    pBuffer ����ָ��    NumToWrite   д�����ݳ���
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void process_update(uint8_t Index, uint8_t *pBuffer,u32 NumToWrite)
{
	FLASH_Write( ADDR_FLASH_SECTOR_6 + ( Index * 512 ), pBuffer, NumToWrite);

}





