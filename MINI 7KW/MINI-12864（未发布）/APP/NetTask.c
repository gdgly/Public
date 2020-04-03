#include "includes.h"
#include "Para.h"
#include "ethernet.h"
#include "gprs.h"
#include "NetTask.h"
#include "usart.h"
static uint8_t UpChannel;
static uint8_t LwipInit = 1;

NETINFO EthInfo={0};  //ȫ�ֱ���
/************************************************************************************************************
** �� �� �� : NetSend
** �������� : ���緢�ͺ���
** ��    �� : sn socketֵ0-1 buffer��������ָ�� len�������ݵĳ���
** ��    �� : ��
** ��    �� :	1���ͳɹ� ����ʧ��
*************************************************************************************************************
*/
uint8_t NetSend(uint8_t sn, uint8_t *buffer, uint16_t len)
{
	uint8_t ret = 0;
	if(UpChannel == ETHLWIP){
		ret = EthSend(sn, buffer, len);
	}else if(UpChannel == GPRS) {
		ret = GprsSend(sn, buffer, len);
	}
	return  ret;
}

/************************************************************************************************************
** �� �� �� : NetRecv
** �������� : ������պ���
** ��    �� : sn socketֵ0-1 buffer��������ָ�� len�������ݵĳ���
** ��    �� : ��
** ��    �� :	���յ����ݵĳ���
*************************************************************************************************************
*/
uint16_t NetRecv(uint8_t sn, uint8_t *buffer, uint16_t len)
{
	uint16_t rlen=0;
	if(UpChannel == ETHLWIP){
		rlen = EthRecv(sn, buffer, len);
	}else if(UpChannel == GPRS) {
		rlen = GprsRecv(sn, buffer, len);
	}
	return  rlen;
}


/************************************************************************************************************
** �� �� �� : NetConn
** �������� : �������Ӻ���
** ��    �� : sn socketֵ0-1 serverip������IP serverport�������˿�
** ��    �� : ��
** ��    �� :	1�ɹ� ����ʧ��
*************************************************************************************************************
*/
uint8_t NetConn(uint8_t sn, uint8_t serverip[4], uint16_t serverport)
{
	uint8_t ret=0;
	if(UpChannel== ETHLWIP){
		ret=EthConn(sn, serverip, serverport);
	}else if(UpChannel == GPRS) {
		ret=GprsCon(sn, serverip, serverport);  
	}
	return  ret;
}


/************************************************************************************************************
** �� �� �� : GetLwipInitStatus
** �������� : ��ȡ	LWIP ��ʼ��״̬
** ��    �� : ��
** ��    �� : ��
** ��    �� :	1  ��ʼ��δͨ��    0 ��ʼ��ͨ�� 
*************************************************************************************************************
*/
uint8_t GetLwipInitStatus(void)
{
	return LwipInit;
}


/************************************************************************************************************
** �� �� �� : NetTask
** �������� : ��������ͨ�����ò���ѡ�����Ӧ�Ĵ���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/

 
void NetTask(void *pdata)
{

	UpChannel = ParaGetUpChannel();
	if(UpChannel == ETHLWIP){
		while( EthInit()); //�ȴ���ʼ�����
		LwipInit = 0;
	}else if(UpChannel == GPRS) {
		LwipInit = EthInit();
		UartInit(GPRSUART,115200);   // ����
		GPRSInit();
		
	}
	while(1){
		if(UpChannel == ETHLWIP){
			EthHandle();
		}else if(UpChannel == GPRS) {
			GprsProc();
		}
		OSTimeDlyHMSM (0, 0, 0, 15);
	}
}
