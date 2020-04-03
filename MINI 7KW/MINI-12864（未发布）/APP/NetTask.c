#include "includes.h"
#include "Para.h"
#include "ethernet.h"
#include "gprs.h"
#include "NetTask.h"
#include "usart.h"
static uint8_t UpChannel;
static uint8_t LwipInit = 1;

NETINFO EthInfo={0};  //全局变量
/************************************************************************************************************
** 函 数 名 : NetSend
** 功能描述 : 网络发送函数
** 输    入 : sn socket值0-1 buffer发送数据指针 len发送数据的长度
** 输    出 : 无
** 返    回 :	1发送成功 否则失败
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
** 函 数 名 : NetRecv
** 功能描述 : 网络接收函数
** 输    入 : sn socket值0-1 buffer接收数据指针 len接收数据的长度
** 输    出 : 无
** 返    回 :	接收到数据的长度
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
** 函 数 名 : NetConn
** 功能描述 : 网络连接函数
** 输    入 : sn socket值0-1 serverip服务器IP serverport服务器端口
** 输    出 : 无
** 返    回 :	1成功 否则失败
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
** 函 数 名 : GetLwipInitStatus
** 功能描述 : 获取	LWIP 初始化状态
** 输    入 : 无
** 输    出 : 无
** 返    回 :	1  初始化未通过    0 初始化通过 
*************************************************************************************************************
*/
uint8_t GetLwipInitStatus(void)
{
	return LwipInit;
}


/************************************************************************************************************
** 函 数 名 : NetTask
** 功能描述 : 网络任务，通过配置参数选择相对应的处理
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/

 
void NetTask(void *pdata)
{

	UpChannel = ParaGetUpChannel();
	if(UpChannel == ETHLWIP){
		while( EthInit()); //等待初始化完成
		LwipInit = 0;
	}else if(UpChannel == GPRS) {
		LwipInit = EthInit();
		UartInit(GPRSUART,115200);   // 串口
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
