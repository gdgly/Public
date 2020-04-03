/****************************************************************************\
 Module:           GPRSͨ��
 instructions:     ���ӻ�ȡ�ź�ǿ�Ⱥ������޸Ĵ���淶
 Version:          1.0.0
 Author:           YuL
 Datetime:         18-09-29
 *****************************************************************************/



#include <sys.h>
#include "usart.h"
#include "timer.h"
#include "includes.h"
#include "NetTask.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gprs.h"
#include "Para.h"
#include "Cfg.h"

static GPRS4GINFO  GprsInfo = {0};
static uint8_t readbuf[1024] = {0};

static uint32_t TimerCALL;
static uint32_t TimerCSQ;
static uint32_t TimerCPIN;
static uint32_t TimerSTART;
/************************************************************************************************************
** �� �� �� : mystrstr
** �������� : ������ͬ�ڴ溯�����ڳ���Ϊ leng �� str �ڴ��в��ҳ���Ϊ len �� msg �ڴ�
** ��    �� : str��Ҫ���ҵ��ڴ�ָ��  msg��֮�Ƚϵ��ַ�ָ��  leng str����   len msg�ڴ泤��
** ��    �� : ��
** ��    �� : 0 δ���ҵ� ���򷵻���Ҫƫ�Ƶ��ֽ���-1
*************************************************************************************************************
*/
static uint32_t mystrstr(uint8_t *str, char *msg, uint32_t leng, uint32_t len)
{
	char *p = msg;
	uint32_t len1 = len;
	uint32_t rlen = 0;
	uint32_t rlen1 = 1;
	
	while (leng)
	{
		if(*str == *msg)
		{
			rlen = rlen1;
			while (len)
			{
				if(leng == 0)
					return 0;
				if (*str == *p){
					rlen1++;
					str++;
					p++;
					leng--;
					len--;
					if(len == 0){
						return rlen;
					}
				}else{
					str++;
					rlen1++;
					leng--;
					len = len1;
					p = msg;
					break;
				}
			}	
		}else{
			str++;
			leng--;
			rlen1++;
			if (len == 0){
				return 0;
			}
		}
	}
	return 0;
}


/************************************************************************************************************
** �� �� �� : GprsReadbuf
** �������� : ��ȡGPRS socket���յ�������
** ��    �� : data:��Ž��յ����ݵ�ָ�룬 ��Ҫ��ȡ�����ݳ���
** ��    �� : ��
** ��    �� : 0 δ�������� ���򷵻ض��������ݳ���
*************************************************************************************************************
*/
static uint16_t GprsReadbuf(uint8_t *data, uint16_t datalen)
{
	uint16_t r_len = 0;
	
	if( (data == NULL) || (datalen == 0)){
		return 0;
	}
	while(GprsInfo.RecvRd != GprsInfo.RecvWr){
		*data = GprsInfo.ReBuf[GprsInfo.RecvRd];
		GprsInfo.RecvRd = (GprsInfo.RecvRd + 1) % ME4GDATAMAXLEN;
		data++;
		if(++r_len >= datalen){
			return r_len;
		}
	}
	return r_len;
}


/************************************************************************************************************
** �� �� �� : Recoverbuf
** �������� : �ָ�GPRS���ڴ���������
** ��    �� : ��Ҫ�ָ������ݳ���
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
static void Recoverbuf(uint16_t len)
{
	if( GprsInfo.RecvRd >= len) {
		GprsInfo.RecvRd = GprsInfo.RecvRd - len;   
	}else{
		GprsInfo.RecvRd = ME4GDATAMAXLEN - (len - GprsInfo.RecvRd );
	}
}


/************************************************************************************************************
** �� �� �� : GprsReadSocket
** �������� : ��GPRS���ڽ��յ������н������������͵����ݲ��洢
** ��    �� : ��
** ��    �� : ��
** ��    �� : 0 û�н����ķ��������������ݣ� 1 �����������ݷ���
*************************************************************************************************************
*/
static uint8_t GprsReadSocket(void)   
{

	uint16_t len = 0;
	char str[30] = {0};
	uint8_t serverip[4];
	uint16_t sport,i;
	uint16_t datalen = 0;
	uint8_t* pdata = NULL;
	uint8_t* pdata1 = NULL;
	uint8_t* pdata2 = NULL;
	uint16_t offset = 0;
	char sn = '1';
	
	memset(readbuf,0,1024);
	ParaGetServerIp(0, serverip);
	sport = ParaGetServerPort(0);
	sprintf(str, ",%d.%d.%d.%d,%d,", (uint16_t)serverip[0], (uint16_t)serverip[1], (uint16_t)serverip[2], (uint16_t)serverip[3], sport);
	len = UartRead(GPRSUART, (uint8_t *)readbuf, 1024);						
	if(len > 0){
		for(i = 0; i < len; i++){
			GprsInfo.ReBuf[GprsInfo.RecvWr] = readbuf[i];
			GprsInfo.RecvWr = (GprsInfo.RecvWr + 1) % ME4GDATAMAXLEN;
			if(GprsInfo.RecvWr == GprsInfo.RecvRd){
				GprsInfo.RecvRd = (GprsInfo.RecvRd + 1) % ME4GDATAMAXLEN;
			}	
		}
	}	
	len = GprsReadbuf((uint8_t *)readbuf, 1024);	
	pdata = readbuf;
	if( len ){
		offset = mystrstr(readbuf, "+", len, 1);  //ECV:
		if(offset){
			if((len +1 -offset) < 11){
				Recoverbuf(len +1 -offset);	
				return 0;
			}else{
				offset = mystrstr(readbuf, "+ZIPRECV:", len, 9);  //ECV:	
				if( offset ){	
					pdata2 = readbuf + offset - 1;
					pdata = readbuf + offset - 1;
					offset = mystrstr(pdata, ",", (uint32_t)(pdata - len), 1);
					if( offset ){        //���յ����׽���
						pdata = pdata + offset - 1;
						sn = *(pdata-1) - '1';
						if(sn < SNNUM){
							pdata += 1;
							offset = mystrstr(pdata, ",", (uint32_t)(pdata - len), 1);
							if( offset ){ //���յ��˷�����IP ��ַ
								pdata = pdata + offset - 1;
								pdata += 1;
								offset = mystrstr(pdata, ",", (uint32_t)(pdata - len), 1);
								if( offset ){		 //���յ��˶˿ں�
									pdata = pdata + offset - 1;							
									pdata1 = pdata + 1;
									pdata += 1;
									offset = mystrstr(pdata, ",", (uint32_t)(pdata - len), 1);
									if( offset ){ //���յ������ݳ���
										pdata = pdata + offset - 1;			
										pdata += 1;
										if(mystrstr(readbuf+(pdata2-readbuf), str, len - (pdata2 - readbuf), strlen(str))){//�ж�IP��ַ�Ͷ˿ں�
											datalen = atoi((const char*)pdata1);  //���ݳ���
											if((len - (pdata - readbuf)) >= datalen + 2 ){ //���ݳ��ȼ��� "\r\n"
												pdata1 = pdata + datalen;
												if(*pdata1 ==  '\r' && *(pdata1 + 1 ) == '\n'){
													for(i = 0; i < datalen; i++){ //  �������������͵�����
														EthInfo.RecvBuf[sn][EthInfo.RecvWr[sn]] = *(pdata + i);  
														EthInfo.RecvWr[sn] = (EthInfo.RecvWr[sn] + 1) % NETDATAMAXLEN;			
														if(EthInfo.RecvWr[sn] == EthInfo.RecvRr[sn]){
															EthInfo.RecvRr[sn]  = (EthInfo.RecvRr[sn]) % BUFFER_LEN;
														}	
													}
													Recoverbuf(len - (pdata1 - readbuf));								
													return 1; 
												}else{
													Recoverbuf(len - (pdata2 - readbuf) + 9);	//�������ݴ���			
													return 0; 
												}
											}else{
												Recoverbuf(len - (pdata2 - readbuf));	//�������ݽ��ղ�����			
												return 0; 
											}	
										}else{
											Recoverbuf(len - (pdata2 - readbuf) + 9); //�����Ǳ�׮���õ�IP��ַ�˿ںŷ���������
										}								
									}
								}
							}
						}
					}
					Recoverbuf(len - (pdata2 - readbuf));				//�������ݽ��ղ�����				
					return 0;//���ݳ��Ȳ���
				}
			}	
		}
//		print("���� len = %d  %s\r\n",len ,readbuf);
		return 0;  //û�н��յ�����������
	}	
	return 0;  //û�ж�������
}



/************************************************************************************************************
** �� �� �� : GPRS_SendCmd
** �������� : GPRS����AT�����ȡ��������
** ��    �� : cmd���͵�AT����ָ�� cmdlen AT�����ַ�������  ack GPRSӦ���ַ���ָ��  acklenӦ���ַ�������  waittime ��Ӧʱ��
** ��    �� : �� 
** ��    �� : 1 �õ���Ԥ�ڵ���Ӧ�ַ�����0  ʧ�� 
*************************************************************************************************************
*/
static uint8_t GPRS_SendCmd(uint8_t *cmd, uint16_t cmdlen, char *ack, uint16_t acklen, uint16_t waittime)
{
	uint32_t TimerCnt = 0; 	
	uint16_t len = 0;
	uint16_t recvlen = 0;
	uint16_t i;
	uint32_t offset;			
	uint8_t* pdata = NULL;
	uint16_t rssi = 0;
	
	UartWrite(GPRSUART, cmd, cmdlen);
	TimerCnt = TimerRead();
//	print("%s\r\n",cmd);
	if(ack && waittime){
		while(((TimerRead() - TimerCnt) * 10) <  waittime){
			OSTimeDlyHMSM(0,0,0,10);
			len = UartRead(GPRSUART, (uint8_t *)readbuf + recvlen, 1024);						
			if(len != 0){
				for(i = 0; i < len; i++){
					GprsInfo.ReBuf[GprsInfo.RecvWr] = readbuf[i + recvlen];
					GprsInfo.RecvWr = (GprsInfo.RecvWr + 1) % ME4GDATAMAXLEN;
					if(GprsInfo.RecvWr == GprsInfo.RecvRd){
						GprsInfo.RecvRd = (GprsInfo.RecvRd + 1) % ME4GDATAMAXLEN;
					}	
				}		
				recvlen += len;	
			}					
			if(recvlen){
				offset = mystrstr(readbuf, (char*)ack, recvlen, acklen);
				if( offset ){
					pdata = readbuf + offset - 1;
					if(memcmp(ack, "+CSQ:", 5) == 0){
						offset = mystrstr(pdata, ",", recvlen - offset + 1, 1);
						if( offset ){
							rssi = atoi((char*)pdata + 5);
							GprsInfo.Signal = rssi;
							if(rssi == 99){
								printf("Error:CSQ:%d,No signal\r\n",rssi);
								OSTimeDlyHMSM(0,0,1,0);
							}
						}
					}
					GprsReadSocket();   
					return 1;		
				}	
			}				
		}
		return 0; 
	}
	return 1;
} 


/************************************************************************************************************
** �� �� �� : GetSignal
** �������� : ��ȡ�ź�ǿ�ȣ�  99 ���ź�   0-31  �ź�Խ����ֵԽ��
** ��    �� : ��
** ��    �� : �� 
** ��    �� : �ź�ǿ��
*************************************************************************************************************
*/
uint8_t GetSignal(void)
{
	return GprsInfo.Signal;
}


/************************************************************************************************************
** �� �� �� : GprsCon_
** �������� : GPRS���ӷ���������
** ��    �� : sn socketֵ0-1 serverip������IP serverport�������˿�
** ��    �� : �� 
** ��    �� : 1�ɹ� ����ʧ�� 
*************************************************************************************************************
*/

static uint8_t GprsCon_(uint8_t sn, uint8_t serverip[4], uint16_t port) //���ⲿ�� API
{
	char Sendbuf[100] = {0};
	char ATBuf[30] = {0};
	static uint8_t ConErr[2];
	
	if(sn >= SNNUM){
		return 0;
	}
	if(EthInfo.LinkFlag != 1)  //���û������
		return 0;

	sprintf(Sendbuf, "AT+ZIPSTAT=%d\r\n", sn + 1);
	sprintf(ATBuf, "+ZIPSTAT: %d,1", sn + 1);
	if(EthInfo.ConnState[sn] == 0){
		if(GPRS_SendCmd((uint8_t*)Sendbuf, strlen(Sendbuf), ATBuf, strlen(ATBuf), 1000)){
			EthInfo.ConnState[sn] = 1;
			ConErr[port] = 0;
			return 1;
		}else{	
			EthInfo.ConnState[sn] = 0;
		}	
	}
	
	if(TimerRead() - TimerSTART > T1S * 30 ){
		TimerSTART = TimerRead();
		if(GPRS_SendCmd((uint8_t*)Sendbuf, strlen(Sendbuf), ATBuf, strlen(ATBuf), 1000)){
			EthInfo.ConnState[sn] = 1;
			ConErr[port] = 0;
			return 1;
		}else{	
		EthInfo.ConnState[sn] = 0;
		}	
	}

	if(EthInfo.ConnState[sn] == 1)  //���û������
		return 1;
	
	
	if(EthInfo.ConnState[sn] == 0){
		sprintf(ATBuf, "+ZIPSTAT: %d,1", sn + 1);
		sprintf(Sendbuf, "AT+ZIPOPEN=%d,0,%d.%d.%d.%d,%d\r\n", sn + 1, serverip[0], serverip[1], serverip[2], serverip[3], port);
		if(GPRS_SendCmd((uint8_t*)Sendbuf, strlen(Sendbuf), ATBuf, strlen(ATBuf), 3000 )){
			EthInfo.ConnState[sn] = 1;
			ConErr[port] = 0;
			return 1;
		}
		ConErr[port]++;
		if(ConErr[port] >= 10){
			ConErr[0] = 0;
			ConErr[1] = 0;
			printf("PORT %d ����%d.%d.%d.%d,%d ʧ��\r\n", port, serverip[0], serverip[1], serverip[2], serverip[3], port);
			printf("ģ������\r\n");
			EthInfo.LinkFlag = 0;
		}
		
		return 0;
	}else{		
		ConErr[port] = 0;
		return 1;
	}
}





/************************************************************************************************************
** �� �� �� : GPRSReset
** �������� : GPRS��ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
static void GPRSIoInit(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOD, GPIO_Pin_4 | GPIO_Pin_3); 
	

}



/************************************************************************************************************
** �� �� �� : GPRSReset
** �������� : GPRS��ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
static void GPRSReset(void)
{
	GPRS_RET_H();
	OSTimeDlyHMSM(0, 0, 3, 0);
	GPRS_RET_L();
	OSTimeDlyHMSM(0, 0, 4, 0);
}


/************************************************************************************************************
** �� �� �� : GPRSInit
** �������� : GPRS��ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
void GPRSInit(void)
{
	GPRSIoInit();
	GPRSReset();
	EthInfo.ConnState[0] = 0;
	for(;;){
		UartFlush(GPRSUART); //������ڻ���
		if( GPRS_SendCmd((uint8_t *)"AT\r\n", 4, "OK", 2, 2000) != 1){		
			EthInfo.LinkFlag = 0;
			printf("Error:NO Module\r\n");
			OSTimeDlyHMSM (0,0,1,0);
			continue;
		}
		GPRS_SendCmd((uint8_t *)"ATE0\r\n", 6, "OK", 2, 200);//�رջ��� 
		GPRS_SendCmd((uint8_t *)"AT+CSQ\r\n", 8, "+CSQ:", 5, 1000); //��ѯ�ź�״̬
		if(GPRS_SendCmd((uint8_t *)"AT+CPIN?\r\n", 10, "+CPIN: READY", 12, 1000) != 1){
			printf("Error:NO SimCard\r\n");
			GPRSReset();
			continue;
		}
		GPRS_SendCmd((uint8_t *)"AT+COPS=0\r\n", 11, "OK\r\n", 4, 1000);//��������ע��״̬
		if(GPRS_SendCmd((uint8_t *)"AT+CREG?\r\n", 10, "+CREG: 0,1", 10, 1000) != 1){//��ѯ����ע��״̬ 
			printf("Error:Network registration failed\r\n");
			OSTimeDlyHMSM (0,0,0,100);
			continue;
		}	
		if(GPRS_SendCmd((uint8_t *)"AT+ZIPCALL?\r\n", 13, "+ZIPCALL: 1,", 12, 3000) != 1){
			OSTimeDlyHMSM (0,0,0,100);
			printf("Error:Failed to get the link\r\n");
			if(GPRS_SendCmd((uint8_t *)"AT+ZIPCALL=1\r\n", 14, "+ZIPCALL: 1,", 12, 4000) == 1){
				OSTimeDlyHMSM (0,0,0,100);
				EthInfo.LinkFlag = 1;
				break;
			}
			continue;
		}
		break;
	}
	EthInfo.LinkFlag = 1;
}




/************************************************************************************************************
** �� �� �� : GprsCon
** �������� : �������ӷ�����
** ��    �� : sn socketֵ0��1  serverip������IP port�������˿�
** ��    �� : ��
** ��    �� : 1�ɹ� ����ʧ��
*************************************************************************************************************
*/
uint8_t GprsCon(uint8_t sn, uint8_t serverip[4], uint16_t port)
{
	if(EthInfo.LinkFlag != 1)  
		return 0;
	if(sn >= SNNUM)
		return 0;
	memcpy(EthInfo.serverip[sn], serverip, 4);
	EthInfo.serverport[sn] = port;
	EthInfo.socketportusedflag[sn] = 1;
	return EthInfo.ConnState[sn];	
}




/************************************************************************************************************
** �� �� �� : GprsSend
** �������� : ���緢�ͺ���
** ��    �� : sn socketֵ0-7 buffer���ͻ����� len���������ֽڳ���
** ��    �� : ��
** ��    �� : 1�ɹ� ����ʧ��
*************************************************************************************************************
*/
uint8_t GprsSend(uint8_t sn, uint8_t *buffer, uint16_t len)
{
	if(sn >= SNNUM){
		return 0;
	}
	if(EthInfo.LinkFlag != 1){
		return 0;
	}
	if(EthInfo.socketportusedflag[sn] == 0){
		return 0;
	}
	if(EthInfo.ConnState[sn] != 1){
		return 0;
	}
	if(len > NETDATAMAXLEN){
		len = NETDATAMAXLEN;
	}
	memcpy(EthInfo.SendBuf[sn], buffer, len);
	EthInfo.SendLen[sn] = len;
	EthInfo.SendCmd[sn] = 1;
	while(EthInfo.SendCmd[sn] == 1){
		OSTimeDlyHMSM (0, 0, 1, 0);
	}
	if(EthInfo.SendState[sn] == 1){
		return 1;
	}
	return 0;
}




/************************************************************************************************************
** �� �� �� : GprsRecv
** �������� : ������պ���
** ��    �� : sn socketֵ0-7 buffer���ջ����� len���������ֽڳ���
** ��    �� : ��
** ��    �� : ʵ�ʽ��յ������ݳ���
*************************************************************************************************************
*/
uint16_t GprsRecv(uint8_t sn, uint8_t *buffer, uint16_t len) 
{
	uint16_t rlen = 0;
	
	if(sn >= SNNUM){
		return 0;
	}
	if(EthInfo.LinkFlag != 1){
		return 0;
	}
	if(EthInfo.socketportusedflag[sn] == 0){
		return 0;
	}
	if(EthInfo.ConnState[sn] != 1){
		return 0;
	}
	while (EthInfo.RecvRr[sn] != EthInfo.RecvWr[sn]){
		*buffer++ = EthInfo.RecvBuf[sn][EthInfo.RecvRr[sn]];
		EthInfo.RecvRr[sn] = (EthInfo.RecvRr[sn] + 1) % NETDATAMAXLEN;
		if (++rlen > len){
			return len;
		}
	}
	return rlen;
}




/************************************************************************************************************
** �� �� �� : GprsProc
** �������� : GPRS���������������
** ��    �� : ��
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/


void GprsProc(void)
{
	uint8_t sn;
	char Cmdbuf[40]={0};
	if(TimerRead() -TimerCALL > T1M){
		TimerCALL = TimerRead();
		if(GPRS_SendCmd((uint8_t *)"AT+ZIPCALL?\r\n", 13, "+ZIPCALL: 1,", 12, 3000) != 1){  // ��ѯ����ע��״̬	
		EthInfo.LinkFlag = 0;
		}
	}
	if(TimerRead() -TimerCPIN > (T1M *3)){
		TimerCPIN = TimerRead();
		if(GPRS_SendCmd((uint8_t *)"AT+CPIN?\r\n", 10, "+CPIN: READY", 12, 1000) != 1){ //���绰��
			EthInfo.LinkFlag = 0;
		}
	}
	if(EthInfo.LinkFlag == 0){			
		GPRSInit();
	}
	if(TimerRead() - TimerCSQ > T1M *3){
		TimerCSQ = TimerRead();
		GPRS_SendCmd((uint8_t *)"AT+CSQ\r\n", 8, "+CSQ:", 5, 1000); //��ѯ�ź�״̬
	}
	if( EthInfo.LinkFlag ){
		for(sn=0; sn < SNNUM; sn++){		
			if(EthInfo.socketportusedflag[sn] == 1){
				GprsCon_(sn, (uint8_t *)EthInfo.serverip[sn], EthInfo.serverport[sn]);
				if( EthInfo.SendCmd[sn] == 1 ){
					EthInfo.SendState[sn] = 0;
					if( EthInfo.ConnState[sn] ){
						sprintf(Cmdbuf, "AT+ZIPSENDRAW=%d,%d\r\n", sn + 1, EthInfo.SendLen[sn] );				
						if( GPRS_SendCmd((uint8_t *)Cmdbuf,strlen(Cmdbuf), ">", 1, 1000) ){//������������
							if( GPRS_SendCmd(EthInfo.SendBuf[sn], EthInfo.SendLen[sn], "IPSENDRAW:", 10, 2000)){
								EthInfo.SendState[sn] = 1;
							}
						}										
					}
				}
				EthInfo.SendCmd[sn] = 0;
			}
		}	
	}
	GprsReadSocket();   
}




/************************************************************************************************************
** �� �� �� : GprsTask
** �������� : GPRS������
** ��    �� : ��
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
void GprsTask(void* pdata)
{
	UartInit(GPRSUART,115200);   // ����
	GPRSInit();
	while(1){	
		GprsProc();
		OSTimeDlyHMSM (0,0,0,10);		
	}
}


