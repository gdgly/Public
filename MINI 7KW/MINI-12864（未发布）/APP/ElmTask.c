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
/****************************************************************************\
 Function:  HT7017Init
 Descript:  ����оƬ��ʼ��
 Input:	    ��	
 Output:    �� 
 Return:    ��
 Author:     
 Datetime:   
 *****************************************************************************/
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
//	GPIO_ResetBits(GPIOD,GPIO_Pin_0);
	Uart5Init(4800);	 
}

 /****************************************************************************\
 Function:  HT7017_UART_Write
 Descript:  ��HT7017��ַд����
 Input:	    address ��ַ��data ����	
 Output:    �� 
 Return:    ��
 Author:    zhangxin
 Datetime:  17-10-20
 *****************************************************************************/
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

 /****************************************************************************\
 Function:  HT7017_UART_Read
 Descript:  HT7017��ַ������
 Input:	    address ��ַ��data ����	
 Output:    �� 
 Return:    ��
 Author:    zhangxin
 Datetime:  17-10-20
 *****************************************************************************/
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



 /****************************************************************************\
 Function:  HT7017_ReadUrms
 Descript:  ����ѹ
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    zhangxin
 Datetime:  17-10-20
 *****************************************************************************/ 

 uint32_t HT7017_ReadUrms(void)
{
	 
 static uint8_t i;
 uint8_t j;
 uint32_t TotalVol = 0;
 uint32_t Temp;
 static uint32_t rawvol[10];
 Temp = HT7017_UART_Read(MPREG_RMSU);
 if( Temp != 0xFFFFFFFF ){
	rawvol[ i % 10 ] = Temp;
	i++;
	if(i == 10)
	 i = 0;
	for(j = 0; j < 10; j++ ){
		TotalVol += rawvol[j];
	}
	ElmInfo.RawVol = TotalVol / 10;
	ElmInfo.Vol = ElmInfo.RawVol / ParaGetVolFactor();
	return ElmInfo.RawVol;
 }
	return ElmInfo.RawVol;
}
 /****************************************************************************\
 Function:  HT7017_ReadI1rms
 Descript:  ������1
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    zhangxin
 Datetime:  17-10-20
 *****************************************************************************/
uint32_t HT7017_ReadI1rms(void)
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

		ElmInfo.RawVol = TotalCur / 5;
		ElmInfo.Cur = TotalCur /ParaGetCurFactor();
		return ElmInfo.RawCur;
	}
	return ElmInfo.RawCur;
}
 /****************************************************************************\
 Function:  HT7017_ReadPower
 Descript:  ������
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    zhangxin
 Datetime:  17-10-20
 *****************************************************************************/
uint32_t HT7017_ReadPower(void)
{
	ElmInfo.RawKw = HT7017_UART_Read(MPREG_POWERP1);
	ElmInfo.Kw = ElmInfo.RawKw;
	return ElmInfo.RawKw;
}
 /****************************************************************************\
 Function:  HT7017_ReadEnergp
 Descript:  ���й�����
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    zhangxin
 Datetime:  17-10-20
 *****************************************************************************/
uint32_t HT7017_ReadEnergp(void)
{
	ElmInfo.RawTotalelc = HT7017_UART_Read(MPREG_ENERGYP); 
//	ElmInfo.Totalelc = (ElmInfo.RawTotalelc *100)/3200;	
	return ElmInfo.RawTotalelc;
}




 /****************************************************************************\
 Function:  HT7017_ReadChipID
 Descript:  ��ChipID
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:    zhangxin
 Datetime:  17-10-20
 *****************************************************************************/
uint32_t HT7017_ReadChipID(void)
{  
	 return HT7017_UART_Read(MPREG_CHIPID);
}

 /****************************************************************************\
 Function:  ElmGetTotalElc
 Descript:  ��HT7017����
 Input:	    ��
 Output:    �� 
 Return:    ��
 Author:     
 Datetime:   
 *****************************************************************************/
uint32_t 	ElmGetTotalElc(uint8_t IsInit)
{
	if(IsInit){
		LastRowTotalelc = ElmInfo.RawTotalelc;
	}
	return (((ElmInfo.RawTotalelc - LastRowTotalelc) &0xFFFFFF) / ParaGetPowerFactor()) + (LastRowTotalelc / ParaGetPowerFactor());
}

uint32_t ElmGetRawTotalElc(void)
{
	return ElmInfo.RawTotalelc;
}




uint32_t 	ElmGetVolA(void)
{
	return ElmInfo.Vol;
}

uint32_t 	ElmGetRawVolA(void)
{
	return ElmInfo.RawVol;
}




uint32_t 	ElmGetCurA(void)
{
	return ElmInfo.Cur;
}

uint32_t 	ElmGetRawCurA(void)
{
	return ElmInfo.RawCur;
}

uint32_t 	ElmGetKw(void)
{
	return ElmInfo.Kw;
}

uint32_t 	ElmGetRawKw(void)
{
	return ElmInfo.RawKw;
}



uint8_t ElmGetCommState(void)
{
	return ElmInfo.State;
}



void ElmEcal(void)
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
** �������� : ���������
** ��    �� : pdata ��ʹ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
uint32_t id;
void ElmTask(void *pdata)
{
	HT7017Init();
	OSTimeDlyHMSM (0, 0, 0, 20);
	ElmEcal();
	while(1){
		HT7017_ReadUrms();
		OSTimeDlyHMSM (0, 0, 0, 15);
		HT7017_ReadI1rms();
		OSTimeDlyHMSM (0, 0, 0, 15);
		HT7017_ReadEnergp();
		OSTimeDlyHMSM (0, 0, 0, 15);
		
		
	}
}

