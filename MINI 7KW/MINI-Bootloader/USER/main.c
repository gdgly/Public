#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "stmflash.h"   
#include "iap.h"	 
#include "FRAM.h"	 

int main(void)
{ 
	FramInit();
	Uart1Init(115200);	 	//串口初始化为115200
	ReadApp2ToApp1(App2Addr, App1Addr, GetApplenth(1));//更新程序
	printf("Jump to App \r\n");  
	INTX_DISABLE();
	iap_load_app(App1Addr);
}

