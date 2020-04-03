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

struct netconn *udpconn;					//TCP CLIENT网络连接结构体
uint16_t  udpremoteport;					//TCP CLIENT网络连接结构体

#define UDPBUFFER_LEN         512
#define SERVERPORT 		2408
#define MYSO_BROADCAST   		0x0020   //UDP广播相关



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
	netconn_connect(udpconn,&server_ipaddr,udpremoteport); 	//连接到远端主机
	sentbuf = netbuf_new();
	netbuf_alloc(sentbuf, len);
	sentbuf->p->payload = (char*) buf;
	err = netconn_send(udpconn,sentbuf);  
	if(err != ERR_OK){
		printf("UDP发送失败\r\n");
		netbuf_delete(sentbuf);      //删除buf
	}else{
		netbuf_delete(sentbuf);      //删除buf
	}
	netconn_disconnect(udpconn);
	
	
}




/************************************************************************************************************
** 函 数 名 : AnalyseMsgFromUdpPlatform
** 功能描述 : 处理UDP通道配置函数
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
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
		udpconn = netconn_new(NETCONN_UDP);  //创建一个UDP链接
		udpconn->recv_timeout = 10;
		netconn_bind(udpconn, IP_ADDR_ANY, SERVERPORT); 
		ip_set_option(udpconn->pcb.ip, MYSO_BROADCAST);
	}
	if(udpconn != NULL){
		netconn_recv(udpconn,&recvbuf); //接收数据
		if(recvbuf != NULL){          //接收到数据
			INTX_DISABLE();
			udpremoteport = recvbuf->port;  //获取远程端口号
			for(q=recvbuf->p;q!=NULL;q=q->next){
				if(q->len > UDPBUFFER_LEN ){
					memcpy( &buffer,q->payload, UDPBUFFER_LEN);   //丟包		
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
