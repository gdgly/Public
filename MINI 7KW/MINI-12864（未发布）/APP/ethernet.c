#include "includes.h"
#include <stdint.h>
#include <string.h>

#include "malloc.h"
#include <string.h>
#include "NetTask.h"
#include "usart.h"
#include "NetTask.h"
#include "Para.h"
#include "timer.h"
#include "ethernet.h"
#include "lan8720.h"

#include "lwip/opt.h"
#include "lwip_comm.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "lwip/ip.h"
#include "sys.h"

struct netconn *tcp_clientconn;					//TCP CLIENT�������ӽṹ��




/************************************************************************************************************
** �� �� �� : EthInit
** �������� : ��̫����ʼ������������LWIP�ں����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	0 ��ʼ���ɹ�  ���� ������
*************************************************************************************************************
*/
uint8_t EthInit(void)
{
	my_mem_init(SRAMIN);	
	return lwip_comm_init();//lwip��ʼ��	
}


/************************************************************************************************************
** �� �� �� : EthConn
** �������� : ��̫�����Ӻ���
** ��    �� : sn socketֵ0-1 serverip������IP serverport�������˿�
** ��    �� : ��
** ��    �� :	1�ɹ� ����ʧ��
*************************************************************************************************************
*/
uint8_t EthConn(uint8_t sn, uint8_t serverip[4], uint16_t serverport)
{
	if(sn >= SNNUM){
		return 0;
	}
	if(EthInfo.LinkFlag != 1){
		return 0;
	}
//	printf("Connect to %d.%d.%d.%d,%d P%d:\r\n", serverip[0], serverip[1], serverip[2], serverip[3], serverport, sn);
	memcpy(EthInfo.serverip[sn], serverip, 4);
	EthInfo.serverport[sn] = serverport;
	EthInfo.socketportusedflag[sn] = 1;
	EthInfo.ConnCmd[sn] = 1;
	while(EthInfo.ConnCmd[sn] == 1){
		OSTimeDlyHMSM (0, 0, 0, 100);
	}
	if(EthInfo.ConnState[sn] == 1){
		printf("Connect success P%d:\r\n", sn);
		return 1;
	} else {
//		printf("Connect fail P%d:\r\n", sn);
		return 0;
	}
}

/************************************************************************************************************
** �� �� �� : EthSend
** �������� : ��̫�����ͺ���
** ��    �� : sn socketֵ0-1 buffer���ͻ����� len���������ֽڳ���
** ��    �� : ��
** ��    �� :	1�ɹ� ����ʧ��
*************************************************************************************************************
*/
uint8_t EthSend(uint8_t sn, uint8_t *buffer, uint16_t len)
{
	sn = 0;
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
** �� �� �� : EthRecv
** �������� : ��̫�����պ���
** ��    �� : sn socketֵ0-1 buffer���ջ����� len���������ֽڳ���
** ��    �� : ��
** ��    �� :	ʵ�ʽ��յ������ݳ���
*************************************************************************************************************
*/
uint16_t EthRecv(uint8_t sn, uint8_t *buffer, uint16_t len)
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
** �� �� �� : EthHandle
** �������� : ��̫��������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void EthHandle(void)
{
	uint8_t sn, ret;
	static ip_addr_t server_ipaddr;

	err_t recv_err;
	OS_CPU_SR cpu_sr;
	struct netbuf *recvbuf = NULL;
	struct pbuf *q;
	sn = 0;
	
	IP4_ADDR(&server_ipaddr, lwipdev.remoteip[0],lwipdev.remoteip[1], lwipdev.remoteip[2],lwipdev.remoteip[3]);
	EthInfo.LinkFlag = 1;
	if(EthInfo.socketportusedflag[sn] == 1){  //���Ӵ���		
		if(LAN8720GetLink() == PHY_Linked_Status ){
			EthInfo.LinkFlag = 1;			
			if(EthInfo.ConnCmd[sn] == 1){
				EthInfo.ConnState[sn] = 0;
				netconn_delete(tcp_clientconn); //ɾ��tcp_clientconn����
				tcp_clientconn = netconn_new(NETCONN_TCP);  //����һ��TCP����		
				if(netconn_connect(tcp_clientconn,&server_ipaddr,ParaGetServerPort(0)) == ERR_OK){				
					tcp_clientconn->recv_timeout = 10;
					EthInfo.ConnState[sn] = 1;
				}else{
					netconn_close(tcp_clientconn);
					EthInfo.ConnState[sn] = 0;
					OSTimeDlyHMSM (0, 0, 2, 0);
				}			
				memset(EthInfo.RecvBuf[sn], 0, NETDATAMAXLEN);
				EthInfo.RecvWr[sn] = 0;
				EthInfo.RecvRr[sn] = 0;
				EthInfo.ConnCmd[sn] = 0;
			}
			
			if(EthInfo.SendCmd[sn] == 1){  //��������
				EthInfo.SendState[sn] = 0;
				if(EthInfo.ConnState[sn] == 1){
					ret = netconn_write(tcp_clientconn ,EthInfo.SendBuf[sn], EthInfo.SendLen[sn],NETCONN_COPY);
					if (ret == ERR_OK){
						EthInfo.SendState[sn] = 1;
					}
				}		
				EthInfo.SendCmd[sn] = 0;
			}
			if(EthInfo.ConnState[sn] == 1){   //����ʱ���պ���
				if((recv_err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK){//���յ�����
					OS_ENTER_CRITICAL(); //���ж�
					for(q=recvbuf->p;q!=NULL;q=q->next){  //����������pbuf����			
						if(q->len > (NETDATAMAXLEN - EthInfo.RecvWr[sn])){
							memcpy( &EthInfo.RecvBuf[sn][EthInfo.RecvWr[sn]],q->payload,  NETDATAMAXLEN - EthInfo.RecvWr[sn]);  
							q->payload = (uint8_t *)q->payload +( NETDATAMAXLEN - EthInfo.RecvWr[sn]);
							memcpy( &EthInfo.RecvBuf[sn][0], q->payload,  (q->len - (NETDATAMAXLEN - EthInfo.RecvWr[sn])) % NETDATAMAXLEN);  
							EthInfo.RecvWr[sn] = q->len - (NETDATAMAXLEN - EthInfo.RecvWr[sn]) % NETDATAMAXLEN;
						}else{
							memcpy( &EthInfo.RecvBuf[sn][EthInfo.RecvWr[sn]],q->payload, q->len);
							EthInfo.RecvWr[sn] = (EthInfo.RecvWr[sn] + q->len) % NETDATAMAXLEN;
						}			
					}
					OS_EXIT_CRITICAL();  //���ж�			
					netbuf_delete(recvbuf);	
				}else if(recv_err == ERR_CLSD){  //�ر�����	
					netconn_close(tcp_clientconn);
					EthInfo.ConnState[sn] = 0;
					printf("�������Ͽ�����\r\n");
				}
			}
		}else{
			EthInfo.LinkFlag = 0;
			if(EthInfo.ConnState[sn] == 1 ){
					netconn_close(tcp_clientconn);
					EthInfo.ConnState[sn] = 0;
					printf("�����Ѱ�\r\n");
			}
		}
	}		
		
	//��ѯlink״̬



}



uint8_t GetLinkStatus(void)
{
	return EthInfo.LinkFlag;
}



