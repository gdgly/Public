#include <stdint.h>
#include "string.h"
#include "sys.h"
#include "usart.h"
#include "Para.h"
#include "ElmTask.h"
#include "includes.h"

 ELMINFO ElmInfo;
static volatile uint8_t ErrRecvTimes;
static uint32_t LastRowTotalelc;



/************************************************************************************************************
** 函 数 名 : HT7017Init
** 功能描述 : HT7017Init 硬件初始化
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void HT7017Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);					
	GPIO_SetBits(GPIOD,GPIO_Pin_0);
	Uart5Init(4800);	 
}



/************************************************************************************************************
** 函 数 名 : HT7017_UART_Read
** 功能描述 : 写 HT7017地址 数据
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
 void HT7017_UART_Write(uint8_t address, uint16_t data)
 {
  uint8_t buf[256];
	uint8_t checksum=0;
	uint8_t dat8;
	uint8_t tmplen;
	uint16_t i;
	 
  buf[0]= HEAD;
	buf[1]=address|0x80;//W+Address
	buf[2]=data>>8;
	buf[3]=data &0xff;
	for(i=0;i<4;i++)
	{
	  checksum += buf[i];      
	}
	buf[4]=~checksum;
	UartFlush(DEV_UART5);
	UartWrite(DEV_UART5, buf, 5);
	
	for (i=0;i<1000;i++){				    // 接收数据
		OSTimeDly(2);	
		tmplen=UartRead(DEV_UART5,&dat8,1);
		if (tmplen != 0){
			if (dat8 == 0x54){
				break;
			}
		}
	}
	if(i == 1000){
		ErrRecvTimes++;
		if(ErrRecvTimes >= 20){
			ElmInfo.State = 1;
			ErrRecvTimes = 20;	
		}	
	}else{
		ElmInfo.State = 0;
		ErrRecvTimes = 0;
	}
}


/************************************************************************************************************
** 函 数 名 : HT7017_UART_Read
** 功能描述 : HT7017地址读数据
** 输    出 : 无
** 返    回 :	返回0xFFFFFFFF 为失败
*************************************************************************************************************
*/
 uint32_t HT7017_UART_Read(uint8_t address)
 {
  uint8_t buf[10];
	uint8_t checksum=0;
	uint8_t data[4];
	uint16_t j;
	uint32_t regdata=0;
	 
	uint8_t  tmpbuf[64]={0};
	uint16_t tmplen=0;
	uint8_t rcvbuf[64]={0};
	uint16_t rcvlen=0;
  buf[0]=HEAD;
	buf[1]=address&0x7F;//R+Address
	UartFlush(DEV_UART5);
	UartWrite(DEV_UART5, buf, 2);	
	for (j=0;j<1000;j++){				    // 接收数据
		OSTimeDly(2);	
		tmplen=UartRead(DEV_UART5, tmpbuf, 100);
		if (tmplen != 0){
			if (rcvlen+tmplen >= 64){
				return 0;
			}
			memcpy(rcvbuf+rcvlen,tmpbuf,tmplen);
			rcvlen += tmplen;
			if(rcvlen >= 4){
				 memcpy(data,rcvbuf,4);
				break;
			}
		}
	}
	if(j == 1000){
		ErrRecvTimes++;
		if(ErrRecvTimes >= 20){
			ElmInfo.State = 1;
			ErrRecvTimes = 20;	
		}	
	}else{
		ElmInfo.State = 0;
		ErrRecvTimes = 0;
	}
		
	checksum =~(buf[0]+buf[1]+data[0]+data[1]+data[2]);
	regdata = 0xFFFFFFFF;
	if(checksum == data[3])
		regdata = ((data[0]<<16)|(data[1]<<8)|data[2])	;
	return regdata;	
}



/************************************************************************************************************
** 函 数 名 : HT7017_ReadUrms
** 功能描述 : 读取芯片电压
** 输    入 : 无
** 输    出 : 无
** 返    回 :	
*************************************************************************************************************
*/
 
static uint32_t HT7017_ReadUrms(void)
{
	 
 static uint8_t i;
 uint8_t j;
 uint32_t TotalVol = 0;
 uint32_t Temp;
 static uint32_t rawvol[5];
 Temp = HT7017_UART_Read(MPREG_RMSU);
 if( Temp != 0xFFFFFFFF ){
	rawvol[ i % 5 ] = Temp;
	i++;
	if(i == 5)
	 i = 0;
	for(j = 0; j < 5; j++ ){
		TotalVol += rawvol[j];
	}
	ElmInfo.RawVol = TotalVol / 5;
	ElmInfo.Vol = ElmInfo.RawVol / ParaGetVolFactor();
	return ElmInfo.RawVol;
 }
	return ElmInfo.RawVol;
}


/************************************************************************************************************
** 函 数 名 : HT7017_ReadI1rms
** 功能描述 : 读取芯片电流
** 输    入 : 无
** 输    出 : 无
** 返    回 :	
*************************************************************************************************************
*/
static uint32_t HT7017_ReadI1rms(void)
{
	static uint32_t rawcur[5];
	static uint8_t i;
	uint8_t j;
	uint32_t TotalCur = 0;
	uint32_t Temp;
	Temp = HT7017_UART_Read(MPREG_RMSI1);
	if( Temp != 0xFFFFFFFF ){
		rawcur[ i % 5 ] = Temp;
		i++; 
		if(i == 5)
		i = 0; 
		for(j = 0; j < 5; j++ ){
			TotalCur += rawcur[j];
		}
		ElmInfo.RawCur = TotalCur / 5;
		ElmInfo.Cur = ElmInfo.RawCur /ParaGetCurFactor();
		return ElmInfo.RawCur;
	}
	return ElmInfo.RawCur;
}


/************************************************************************************************************
** 函 数 名 : HT7017_ReadPower
** 功能描述 : 读取芯片功率值
** 输    入 : 无
** 输    出 : 无
** 返    回 :	电压值
*************************************************************************************************************
*/
uint32_t HT7017_ReadPower(void)
{
	ElmInfo.RawKw = HT7017_UART_Read(MPREG_POWERP1);
	ElmInfo.Kw = ElmInfo.RawKw;
	return ElmInfo.RawKw;
}
/************************************************************************************************************
** 函 数 名 : HT7017_ReadEnergp
** 功能描述 : 读取芯片能力值
** 输    入 : 无
** 输    出 : 无
** 返    回 :	电压值
*************************************************************************************************************
*/
uint32_t HT7017_ReadEnergp(void)
{
	uint32_t Temp = HT7017_UART_Read(MPREG_ENERGYP); 
	if(Temp != 0xFFFFFFFF){
		ElmInfo.RawTotalelc = Temp;
	}
	return ElmInfo.RawTotalelc;
}




/************************************************************************************************************
** 函 数 名 : HT7017_ReadChipID
** 功能描述 : 读取芯片ChipID, 默认值为 0x7053B0
** 输    入 : 无
** 输    出 : 无
** 返    回 :	电压值
*************************************************************************************************************
*/
uint32_t HT7017_ReadChipID(void)
{  
	 return HT7017_UART_Read(MPREG_CHIPID);
}

/************************************************************************************************************
** 函 数 名 : ElmGetTotalElc
** 功能描述 : 获取计量芯片电能值 单位 0.01 KW/H
** 输    入 : 无
** 输    出 : 无
** 返    回 :	电压值
*************************************************************************************************************
*/
uint32_t 	ElmGetTotalElc(uint8_t IsInit)
{
	if(IsInit){
		LastRowTotalelc = ElmInfo.RawTotalelc;
	}
	return (((ElmInfo.RawTotalelc - LastRowTotalelc) &0xFFFFFF) / ParaGetPowerFactor()) + (LastRowTotalelc / ParaGetPowerFactor());
}


/************************************************************************************************************
** 函 数 名 : ElmGetRawTotalElc
** 功能描述 : 获取计量芯片电能值原始值
** 输    入 : 无
** 输    出 : 无
** 返    回 :	电压值
*************************************************************************************************************
*/
uint32_t ElmGetRawTotalElc(void)
{
	return ElmInfo.RawTotalelc;
}



/************************************************************************************************************
** 函 数 名 : ElmGetVolA
** 功能描述 : 获取电压值 ，单位 0.1V
** 输    入 : 无
** 输    出 : 无
** 返    回 :	电压值
*************************************************************************************************************
*/
uint32_t 	ElmGetVolA(void)
{
	return ElmInfo.Vol;
}


/************************************************************************************************************
** 函 数 名 : ElmGetRawVolA
** 功能描述 : 获取电压原始值
** 输    入 : 无
** 输    出 : 无
** 返    回 :	电压原始值，非实际值
*************************************************************************************************************
*/
uint32_t 	ElmGetRawVolA(void)
{
	return ElmInfo.RawVol;
}



/************************************************************************************************************
** 函 数 名 : ElmGetCurA
** 功能描述 : 获取电流值 单位 0.001A
** 输    入 : 无
** 输    出 : 无
** 返    回 :	电流值
*************************************************************************************************************
*/
uint32_t 	ElmGetCurA(void)
{
	return ElmInfo.Cur;
}


/************************************************************************************************************
** 函 数 名 : ElmGetRawCurA
** 功能描述 : 获取电流原始值
** 输    入 : 无
** 输    出 : 无
** 返    回 :	电流原始值，需要除以 K值得到实际值
*************************************************************************************************************
*/
uint32_t 	ElmGetRawCurA(void)
{
	return ElmInfo.RawCur;
}


/************************************************************************************************************
** 函 数 名 : ElmGetKw
** 功能描述 : 获取功率
** 输    入 : 无
** 输    出 : 无
** 返    回 :	功率
*************************************************************************************************************
*/
uint32_t 	ElmGetKw(void)
{
	return ElmInfo.Kw;
}
/************************************************************************************************************
** 函 数 名 : ElmGetRawKw
** 功能描述 : 获取功率原始值
** 输    入 : 无
** 输    出 : 无
** 返    回 :	功率原始值：需要转换
*************************************************************************************************************
*/
uint32_t 	ElmGetRawKw(void)
{
	return ElmInfo.RawKw;
}


/************************************************************************************************************
** 函 数 名 : ElmGetCommState
** 功能描述 : 获取计量芯片状态
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
uint8_t ElmGetCommState(void)
{
	return ElmInfo.State;
}


/************************************************************************************************************
** 函 数 名 : ElmInit
** 功能描述 : 计量芯片相关初始化
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
static void ElmInit(void)
{

	GPIO_ResetBits(GPIOD,GPIO_Pin_0);
	OSTimeDlyHMSM (0, 0, 0, 20);
	HT7017_UART_Write(ECADR_SRST, 0x55); //复位
	OSTimeDlyHMSM (0, 0, 2, 0);
	HT7017_UART_Write(ECADR_WPREG, 0xA6); //写保护打开

	OSTimeDlyHMSM (0, 0, 0, 20);	
	HT7017_UART_Write(ECADR_HFCONST, 0xA6);  //0xA6硬件比例系数计算            7.
	OSTimeDlyHMSM (0, 0, 0, 20);
	HT7017_UART_Write(ECADR_ADCCON, 0x00);  //0x00 ADC通道增益  电压1倍   电流1 倍
	OSTimeDlyHMSM (0, 0, 0, 20);
	GPIO_SetBits(GPIOD,GPIO_Pin_0);
}

/************************************************************************************************************
** 函 数 名 : ElmTask
** 功能描述 : 读计量芯片电表任务
** 输    入 : pdata 不使用
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ElmTask(void *pdata)
{
	HT7017Init();
	OSTimeDlyHMSM (0, 0, 0, 20);
	ElmInit();
	while(1){
		HT7017_ReadUrms();
		OSTimeDlyHMSM (0, 0, 0, 15);
		HT7017_ReadI1rms();
		OSTimeDlyHMSM (0, 0, 0, 15);
		HT7017_ReadEnergp();
		OSTimeDlyHMSM (0, 0, 0, 15);
		
		
	}
}

