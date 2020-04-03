#include "sys.h"
#include "para.h"
#include "includes.h"
#include "lwip/opt.h"
#include "lwip_comm.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "lwip/ip.h"

#include "lan8720.h"
#include "NetTask.h"

struct netconn *udpconn;					//TCP CLIENT�������ӽṹ��
uint16_t  udpremoteport;					//TCP CLIENT�������ӽṹ��

#define UDPBUFFER_LEN         512
#define SERVERPORT 		2408
#define MYSO_BROADCAST   		0x0020   //UDP�㲥���



void UDPSend(uint8_t* buf, uint16_t len)
{
	
	ip_addr_t server_ipaddr;
	err_t err;
	static struct netbuf  *sentbuf;

	if(LAN8720GetLink() != PHY_Linked_Status )
		return;
	if(udpconn == NULL )
		return;
	IP4_ADDR(&server_ipaddr, 255, 255, 255, 255);
	netconn_connect(udpconn,&server_ipaddr,udpremoteport); 	//���ӵ�Զ������
	sentbuf = netbuf_new();
	netbuf_alloc(sentbuf, len);
	sentbuf->p->payload = (char*) buf;
	err = netconn_send(udpconn,sentbuf);  
	if(err != ERR_OK){
		printf("UDP����ʧ��\r\n");
		netbuf_delete(sentbuf);      //ɾ��buf
	}else{
		netbuf_delete(sentbuf);      //ɾ��buf
	}
	netconn_disconnect(udpconn);
	
	
}




/************************************************************************************************************
** �� �� �� : AnalyseMsgFromUdpPlatform
** �������� : ����UDPͨ�����ú���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void AnalyseMsgFromUdpPlatform(void)
{
	static struct netbuf  *recvbuf;
	struct pbuf *q;
	uint8_t buffer[UDPBUFFER_LEN];
	uint16_t recvlen, i; 
	if(GetLwipInitStatus())
		return;
	if(LAN8720GetLink() != PHY_Linked_Status )
		return;
	if(udpconn == NULL){
		udpconn = netconn_new(NETCONN_UDP);  //����һ��UDP����
		udpconn->recv_timeout = 10;
		netconn_bind(udpconn, IP_ADDR_ANY, SERVERPORT); 
		ip_set_option(udpconn->pcb.ip, MYSO_BROADCAST);
	}
	if(udpconn != NULL){
		netconn_recv(udpconn,&recvbuf); //��������
		if(recvbuf != NULL){          //���յ�����
			INTX_DISABLE();
			udpremoteport = recvbuf->port;  //��ȡԶ�̶˿ں�
			for(q=recvbuf->p;q!=NULL;q=q->next){
				if(q->len > UDPBUFFER_LEN ){
					memcpy( &buffer,q->payload, UDPBUFFER_LEN);   //�G��		
					recvlen = UDPBUFFER_LEN;
				}else{
					memcpy( &buffer,q->payload, q->len);
					recvlen = q->len;
				}
			}
			netbuf_delete(recvbuf);
			INTX_ENABLE();
			for (i = 0; i < recvlen; i++){
				if (ConfigProtocolFrameCheck(buffer + i, PARACHANNEL_UDP) == 1){
					if ((buffer[i + 1 + 33] & 0x1f) ==0x01){
						DebugPlatformReadDataAnalyse(&buffer[i], PARACHANNEL_UDP);
					} else if ((buffer[i + 1 + 33] & 0x1f) ==0x11){
						DebugPlatformWriteDataAnalyse(&buffer[i], PARACHANNEL_UDP);
					}
					memset(buffer, 0, sizeof(buffer));
					break;
				}
			}
			recvlen = 0;
		}
	}	
}
