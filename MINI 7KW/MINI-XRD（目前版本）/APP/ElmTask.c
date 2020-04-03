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
** �� �� �� : HT7017Init
** �������� : HT7017Init Ӳ����ʼ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void HT7017Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);					
	GPIO_SetBits(GPIOD,GPIO_Pin_0);
	Uart5Init(4800);	 
}



/************************************************************************************************************
** �� �� �� : HT7017_UART_Read
** �������� : д HT7017��ַ ����
** ��    �� : ��
** ��    �� :	��
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
	
	for (i=0;i<1000;i++){				    // ��������
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
** �� �� �� : HT7017_UART_Read
** �������� : HT7017��ַ������
** ��    �� : ��
** ��    �� :	����0xFFFFFFFF Ϊʧ��
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
	for (j=0;j<1000;j++){				    // ��������
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
** �� �� �� : HT7017_ReadUrms
** �������� : ��ȡоƬ��ѹ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	
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
** �� �� �� : HT7017_ReadI1rms
** �������� : ��ȡоƬ����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	
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
** �� �� �� : HT7017_ReadPower
** �������� : ��ȡоƬ����ֵ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��ѹֵ
*************************************************************************************************************
*/
uint32_t HT7017_ReadPower(void)
{
	ElmInfo.RawKw = HT7017_UART_Read(MPREG_POWERP1);
	ElmInfo.Kw = ElmInfo.RawKw;
	return ElmInfo.RawKw;
}
/************************************************************************************************************
** �� �� �� : HT7017_ReadEnergp
** �������� : ��ȡоƬ����ֵ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��ѹֵ
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
** �� �� �� : HT7017_ReadChipID
** �������� : ��ȡоƬChipID, Ĭ��ֵΪ 0x7053B0
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��ѹֵ
*************************************************************************************************************
*/
uint32_t HT7017_ReadChipID(void)
{  
	 return HT7017_UART_Read(MPREG_CHIPID);
}

/************************************************************************************************************
** �� �� �� : ElmGetTotalElc
** �������� : ��ȡ����оƬ����ֵ ��λ 0.01 KW/H
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��ѹֵ
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
** �� �� �� : ElmGetRawTotalElc
** �������� : ��ȡ����оƬ����ֵԭʼֵ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��ѹֵ
*************************************************************************************************************
*/
uint32_t ElmGetRawTotalElc(void)
{
	return ElmInfo.RawTotalelc;
}



/************************************************************************************************************
** �� �� �� : ElmGetVolA
** �������� : ��ȡ��ѹֵ ����λ 0.1V
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��ѹֵ
*************************************************************************************************************
*/
uint32_t 	ElmGetVolA(void)
{
	return ElmInfo.Vol;
}


/************************************************************************************************************
** �� �� �� : ElmGetRawVolA
** �������� : ��ȡ��ѹԭʼֵ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��ѹԭʼֵ����ʵ��ֵ
*************************************************************************************************************
*/
uint32_t 	ElmGetRawVolA(void)
{
	return ElmInfo.RawVol;
}



/************************************************************************************************************
** �� �� �� : ElmGetCurA
** �������� : ��ȡ����ֵ ��λ 0.001A
** ��    �� : ��
** ��    �� : ��
** ��    �� :	����ֵ
*************************************************************************************************************
*/
uint32_t 	ElmGetCurA(void)
{
	return ElmInfo.Cur;
}


/************************************************************************************************************
** �� �� �� : ElmGetRawCurA
** �������� : ��ȡ����ԭʼֵ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	����ԭʼֵ����Ҫ���� Kֵ�õ�ʵ��ֵ
*************************************************************************************************************
*/
uint32_t 	ElmGetRawCurA(void)
{
	return ElmInfo.RawCur;
}


/************************************************************************************************************
** �� �� �� : ElmGetKw
** �������� : ��ȡ����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	����
*************************************************************************************************************
*/
uint32_t 	ElmGetKw(void)
{
	return ElmInfo.Kw;
}
/************************************************************************************************************
** �� �� �� : ElmGetRawKw
** �������� : ��ȡ����ԭʼֵ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	����ԭʼֵ����Ҫת��
*************************************************************************************************************
*/
uint32_t 	ElmGetRawKw(void)
{
	return ElmInfo.RawKw;
}


/************************************************************************************************************
** �� �� �� : ElmGetCommState
** �������� : ��ȡ����оƬ״̬
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
uint8_t ElmGetCommState(void)
{
	return ElmInfo.State;
}


/************************************************************************************************************
** �� �� �� : ElmInit
** �������� : ����оƬ��س�ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void ElmInit(void)
{

	GPIO_ResetBits(GPIOD,GPIO_Pin_0);
	OSTimeDlyHMSM (0, 0, 0, 20);
	HT7017_UART_Write(ECADR_SRST, 0x55); //��λ
	OSTimeDlyHMSM (0, 0, 2, 0);
	HT7017_UART_Write(ECADR_WPREG, 0xA6); //д������

	OSTimeDlyHMSM (0, 0, 0, 20);	
	HT7017_UART_Write(ECADR_HFCONST, 0xA6);  //0xA6Ӳ������ϵ������            7.
	OSTimeDlyHMSM (0, 0, 0, 20);
	HT7017_UART_Write(ECADR_ADCCON, 0x00);  //0x00 ADCͨ������  ��ѹ1��   ����1 ��
	OSTimeDlyHMSM (0, 0, 0, 20);
	GPIO_SetBits(GPIOD,GPIO_Pin_0);
}

/************************************************************************************************************
** �� �� �� : ElmTask
** �������� : ������оƬ�������
** ��    �� : pdata ��ʹ��
** ��    �� : ��
** ��    �� :	��
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

