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

struct netconn *tcp_clientconn;					//TCP CLIENT网络连接结构体




/************************************************************************************************************
** 函 数 名 : EthInit
** 功能描述 : 以太网初始化函数，包括LWIP内核相关
** 输    入 : 无
** 输    出 : 无
** 返    回 :	0 初始化成功  其他 ：错误
*************************************************************************************************************
*/
uint8_t EthInit(void)
{
	my_mem_init(SRAMIN);	
	return lwip_comm_init();//lwip初始化	
}


/************************************************************************************************************
** 函 数 名 : EthConn
** 功能描述 : 以太网连接函数
** 输    入 : sn socket值0-1 serverip服务器IP serverport服务器端口
** 输    出 : 无
** 返    回 :	1成功 否则失败
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
** 函 数 名 : EthSend
** 功能描述 : 以太网发送函数
** 输    入 : sn socket值0-1 buffer发送缓冲区 len发送数据字节长度
** 输    出 : 无
** 返    回 :	1成功 否则失败
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
** 函 数 名 : EthRecv
** 功能描述 : 以太网接收函数
** 输    入 : sn socket值0-1 buffer接收缓冲区 len接收数据字节长度
** 输    出 : 无
** 返    回 :	实际接收到的数据长度
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
** 函 数 名 : EthHandle
** 功能描述 : 以太网处理函数
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
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
	if(EthInfo.socketportusedflag[sn] == 1){  //连接处理		
		if(LAN8720GetLink() == PHY_Linked_Status ){
			EthInfo.LinkFlag = 1;			
			if(EthInfo.ConnCmd[sn] == 1){
				EthInfo.ConnState[sn] = 0;
				netconn_delete(tcp_clientconn); //删除tcp_clientconn连接
				tcp_clientconn = netconn_new(NETCONN_TCP);  //创建一个TCP链接		
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
			
			if(EthInfo.SendCmd[sn] == 1){  //发送数据
				EthInfo.SendState[sn] = 0;
				if(EthInfo.ConnState[sn] == 1){
					ret = netconn_write(tcp_clientconn ,EthInfo.SendBuf[sn], EthInfo.SendLen[sn],NETCONN_COPY);
					if (ret == ERR_OK){
						EthInfo.SendState[sn] = 1;
					}
				}		
				EthInfo.SendCmd[sn] = 0;
			}
			if(EthInfo.ConnState[sn] == 1){   //连接时接收函数
				if((recv_err = netconn_recv(tcp_clientconn,&recvbuf)) == ERR_OK){//接收到数据
					OS_ENTER_CRITICAL(); //关中断
					for(q=recvbuf->p;q!=NULL;q=q->next){  //遍历完整个pbuf链表			
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
					OS_EXIT_CRITICAL();  //开中断			
					netbuf_delete(recvbuf);	
				}else if(recv_err == ERR_CLSD){  //关闭连接	
					netconn_close(tcp_clientconn);
					EthInfo.ConnState[sn] = 0;
					printf("服务器断开连接\r\n");
				}
			}
		}else{
			EthInfo.LinkFlag = 0;
			if(EthInfo.ConnState[sn] == 1 ){
					netconn_close(tcp_clientconn);
					EthInfo.ConnState[sn] = 0;
					printf("网线已拔\r\n");
			}
		}
	}		
		
	//查询link状态



}



uint8_t GetLinkStatus(void)
{
	return EthInfo.LinkFlag;
}



