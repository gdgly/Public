#include "stmflash.h"
#include "delay.h"
#include "usart.h" 


 
//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}  
//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~11,即addr所在的扇区
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
** 函 数 名 : STMFLASH_ReadAndWrite
** 功能描述 : 从FLASH  ReadAddr 读数据写到 FLASH WriteAddr
** 输    入 : ReadAddr:读地址   WriteAddr:写地址   len: 长度  字节
** 输    出 : 无
** 返    回 :	Applenth
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
	
	FLASH_Unlock();									//解锁 
  FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
	for(i=0;i<len;i++){
		Temp = STMFLASH_ReadWord(ReadAddr);
		FLASH_ProgramWord(WriteAddr,Temp);
	  WriteAddr+=4;//地址增加4.
		ReadAddr+=4;//偏移4个字节.	
	}
	FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
}	

/************************************************************************************************************
** 函 数 名 : EraseAPP1
** 功能描述 : 擦除运行程序扇区
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void EraseAPP1(void)
{
	FLASH_Unlock();									//解锁 
  FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
	
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_1),VoltageRange_3);
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_2),VoltageRange_3);
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_3),VoltageRange_3);
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_4),VoltageRange_3);
	FLASH_EraseSector(STMFLASH_GetFlashSector(ADDR_FLASH_SECTOR_5),VoltageRange_3);
	
	FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
}

/************************************************************************************************************
** 函 数 名 : ReadAppAndWrite
** 功能描述 : 
** 输    入 : ReadAddr:读地址   WriteAddr:写地址   len: 长度
** 输    出 : 无
** 返    回 :	Applenth
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











