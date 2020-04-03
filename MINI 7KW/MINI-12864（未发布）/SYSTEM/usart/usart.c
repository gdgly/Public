#include "sys.h"
#include "usart.h"	
#include "includes.h"					//ucos 使用	  


volatile static UARTBUF UartBuf[UART_MAX];

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif



void printfx(uint8_t *data,uint16_t datalen)
{
//    uint16_t i=0;
//    for(i=0;i<datalen;i++)
//    {
//        printf("%02X ",data[i]);
//    }
    printf("\r\n");
}

//初始化IO 串口1 
//bound:波特率
void Uart1Init(u32 bound)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	USART_DeInit(USART1);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
		//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_Cmd(USART1, ENABLE);  //使能串口1 
	



}


void USART1_IRQHandler(void)                	//串口1中断服务程序
{
#if SYSTEM_SUPPORT_UCOS  //使用UCOS操作系统
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		UartBuf[DEV_UART1].buf[UartBuf[DEV_UART1].Wr] =USART1->DR;
		UartBuf[DEV_UART1].Wr = (UartBuf[DEV_UART1].Wr +1)%BUFFER_LEN;
		if(UartBuf[DEV_UART1].Wr == UartBuf[DEV_UART1].Rd ) {
			UartBuf[DEV_UART1].Rd = (UartBuf[DEV_UART1].Rd +1)%BUFFER_LEN;
		}			   		 
  } 
 #if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//退出中断
#endif
} 



void Uart2Init(u32 bound)
{
   //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	USART_DeInit(USART2);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); //GPIOD5复用为USART1
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); //GPIOD6复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; //GPIOD5与GPIOD6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化PD5，PD6

   //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口1
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	
	//USART2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_Cmd(USART2, ENABLE);  //使能串口1 
}


 
void USART2_IRQHandler(void)                	//串口1中断服务程序
{
#if SYSTEM_SUPPORT_UCOS  //使用UCOS操作系统
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		UartBuf[DEV_UART2].buf[UartBuf[DEV_UART2].Wr] = USART2->DR;
		UartBuf[DEV_UART2].Wr = (UartBuf[DEV_UART2].Wr + 1) % BUFFER_LEN;
		if(UartBuf[DEV_UART2].Wr == UartBuf[DEV_UART2].Rd ) {
			UartBuf[DEV_UART2].Rd = (UartBuf[DEV_UART2].Rd + 1) % BUFFER_LEN;
		}			   		 
  } 
 #if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//退出中断
#endif
} 




/////串口4配置
void Uart4Init(u32 bound) 
{   
	GPIO_InitTypeDef GPIO_InitStructure; 
	USART_InitTypeDef USART_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure; 
 
	USART_DeInit(UART4);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);    

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOC10复用为UART4
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOC11复用为UART4	

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOC10 与  GPIOC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PC10，PC11

	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);        

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(UART4, &USART_InitStructure); 
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART4, ENABLE);                  
}



 
void UART4_IRQHandler(void)                	//串口1中断服务程序
{
#if SYSTEM_SUPPORT_UCOS  //使用UCOS操作系统
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		UartBuf[DEV_UART4].buf[UartBuf[DEV_UART4].Wr] = UART4->DR;
		UartBuf[DEV_UART4].Wr = (UartBuf[DEV_UART4].Wr + 1) % BUFFER_LEN;
		if(UartBuf[DEV_UART4].Wr == UartBuf[DEV_UART4].Rd ) {
			UartBuf[DEV_UART4].Rd = (UartBuf[DEV_UART4].Rd + 1) % BUFFER_LEN;
		}			   		 
  } 
 #if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//退出中断
#endif
} 



/////串口4配置
void Uart5Init(u32 bound) 
{   
	GPIO_InitTypeDef GPIO_InitStructure; 
	USART_InitTypeDef USART_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure; 
 
	USART_DeInit(UART5);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD , ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);    

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOC10复用为UART4
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); //GPIOC11复用为UART4	

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  //GPIOC12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PC12
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  //GPIOD2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化PD2  
	
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);        

	USART_InitStructure.USART_BaudRate = bound; //波特率；
	USART_InitStructure.USART_WordLength = USART_WordLength_9b; //数据位8位；
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
	USART_InitStructure.USART_Parity = USART_Parity_Even ; //偶校验位；
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);//配置串口参数；

	USART_Init(UART5, &USART_InitStructure); 
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART5, ENABLE);                  
}



void UART5_IRQHandler(void)                	//串口1中断服务程序
{
#if SYSTEM_SUPPORT_UCOS  //使用UCOS操作系统
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET){  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		UartBuf[DEV_UART5].buf[UartBuf[DEV_UART5].Wr] = UART5->DR;
		UartBuf[DEV_UART5].Wr = (UartBuf[DEV_UART5].Wr + 1) % BUFFER_LEN;
		if(UartBuf[DEV_UART5].Wr == UartBuf[DEV_UART5].Rd ) {
			UartBuf[DEV_UART5].Rd = (UartBuf[DEV_UART5].Rd + 1) % BUFFER_LEN;
		}			   		 
  } 
 #if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//退出中断
#endif
} 

/****************************************************************************\
 Function:  UartWrite
 Descript:  写串口数据
 Input:	    串口号，数据ＢＵＦ，长度
 Output:    无 
 Return:    实际写的数据长度
 *****************************************************************************/
uint16_t UartWrite( uint8_t no, uint8_t *data, uint16_t datalen)
{
	uint16_t i=0;
	if((no>=UART_MAX) || (data==NULL) || (datalen==0)) {
		return 0; 
	}
	if(no == DEV_UART1){
		for(i=0;i<datalen;i++){
			USART1->DR = *(data+i);
			while((USART1->SR&0X40)==0);	   
		}
	}else if(no == DEV_UART2){
		for(i=0;i<datalen;i++){
			USART2->DR = *(data+i);
			while((USART2->SR&0X40)==0);	   
		}
	}else if(no == DEV_UART4){
		for(i=0;i<datalen;i++){
			UART4->DR = *(data+i);
			while((UART4->SR&0X40)==0);	   
		}
	}else if(no == DEV_UART5){
		for(i=0;i<datalen;i++){
			UART5->DR = *(data+i);
			while((UART5->SR&0X40)==0);	   
		}
	}
	return datalen;
}



uint16_t UartRead(uint8_t no,uint8_t *data, uint16_t datalen)
{
	uint16_t r_len=0;

	if((no>=UART_MAX) || (data==NULL) || (datalen==0)) 
	{
        return 0;
	}

	while(UartBuf[no].Rd != UartBuf[no].Wr)
    {
		*data = UartBuf[no].buf[UartBuf[no].Rd];
		UartBuf[no].Rd = (UartBuf[no].Rd+1) % BUFFER_LEN;
		data++;
		if(++r_len >= datalen)
        {
            return r_len;
		}
	}

	return r_len;
}


void UartFlush( uint8_t no)
{
	if(no >= UART_MAX) 
	{
		return ; 
	}

	UartBuf[no].Wr=UartBuf[no].Rd=0;
   
}



void UartInit(uint8_t no, uint32_t bound)
{
	if(no == DEV_UART1)
		Uart1Init(bound);
	else if(no == DEV_UART2)
		Uart2Init(bound);
	else if(no == DEV_UART4)
		Uart4Init(bound);
	else if(no == DEV_UART5)
		Uart5Init(bound);
}




