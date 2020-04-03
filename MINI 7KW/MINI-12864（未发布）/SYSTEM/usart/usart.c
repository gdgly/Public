#include "sys.h"
#include "usart.h"	
#include "includes.h"					//ucos ʹ��	  


volatile static UARTBUF UartBuf[UART_MAX];

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
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

//��ʼ��IO ����1 
//bound:������
void Uart1Init(u32 bound)
{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	USART_DeInit(USART1);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
		//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�
	USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	



}


void USART1_IRQHandler(void)                	//����1�жϷ������
{
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		UartBuf[DEV_UART1].buf[UartBuf[DEV_UART1].Wr] =USART1->DR;
		UartBuf[DEV_UART1].Wr = (UartBuf[DEV_UART1].Wr +1)%BUFFER_LEN;
		if(UartBuf[DEV_UART1].Wr == UartBuf[DEV_UART1].Rd ) {
			UartBuf[DEV_UART1].Rd = (UartBuf[DEV_UART1].Rd +1)%BUFFER_LEN;
		}			   		 
  } 
 #if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//�˳��ж�
#endif
} 



void Uart2Init(u32 bound)
{
   //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	USART_DeInit(USART2);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); //GPIOD5����ΪUSART1
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); //GPIOD6����ΪUSART1
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; //GPIOD5��GPIOD6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD,&GPIO_InitStructure); //��ʼ��PD5��PD6

   //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������1
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	
	//USART2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�
	USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���1 
}


 
void USART2_IRQHandler(void)                	//����1�жϷ������
{
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		UartBuf[DEV_UART2].buf[UartBuf[DEV_UART2].Wr] = USART2->DR;
		UartBuf[DEV_UART2].Wr = (UartBuf[DEV_UART2].Wr + 1) % BUFFER_LEN;
		if(UartBuf[DEV_UART2].Wr == UartBuf[DEV_UART2].Rd ) {
			UartBuf[DEV_UART2].Rd = (UartBuf[DEV_UART2].Rd + 1) % BUFFER_LEN;
		}			   		 
  } 
 #if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//�˳��ж�
#endif
} 




/////����4����
void Uart4Init(u32 bound) 
{   
	GPIO_InitTypeDef GPIO_InitStructure; 
	USART_InitTypeDef USART_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure; 
 
	USART_DeInit(UART4);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);    

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOC10����ΪUART4
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOC11����ΪUART4	

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOC10 ��  GPIOC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PC10��PC11

	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);        

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(UART4, &USART_InitStructure); 
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART4, ENABLE);                  
}



 
void UART4_IRQHandler(void)                	//����1�жϷ������
{
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		UartBuf[DEV_UART4].buf[UartBuf[DEV_UART4].Wr] = UART4->DR;
		UartBuf[DEV_UART4].Wr = (UartBuf[DEV_UART4].Wr + 1) % BUFFER_LEN;
		if(UartBuf[DEV_UART4].Wr == UartBuf[DEV_UART4].Rd ) {
			UartBuf[DEV_UART4].Rd = (UartBuf[DEV_UART4].Rd + 1) % BUFFER_LEN;
		}			   		 
  } 
 #if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//�˳��ж�
#endif
} 



/////����4����
void Uart5Init(u32 bound) 
{   
	GPIO_InitTypeDef GPIO_InitStructure; 
	USART_InitTypeDef USART_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure; 
 
	USART_DeInit(UART5);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD , ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);    

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOC10����ΪUART4
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); //GPIOC11����ΪUART4	

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  //GPIOC12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PC12
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  //GPIOD2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD,&GPIO_InitStructure); //��ʼ��PD2  
	
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);        

	USART_InitStructure.USART_BaudRate = bound; //�����ʣ�
	USART_InitStructure.USART_WordLength = USART_WordLength_9b; //����λ8λ��
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //ֹͣλ1λ��
	USART_InitStructure.USART_Parity = USART_Parity_Even ; //żУ��λ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);//���ô��ڲ�����

	USART_Init(UART5, &USART_InitStructure); 
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	USART_Cmd(UART5, ENABLE);                  
}



void UART5_IRQHandler(void)                	//����1�жϷ������
{
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET){  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		UartBuf[DEV_UART5].buf[UartBuf[DEV_UART5].Wr] = UART5->DR;
		UartBuf[DEV_UART5].Wr = (UartBuf[DEV_UART5].Wr + 1) % BUFFER_LEN;
		if(UartBuf[DEV_UART5].Wr == UartBuf[DEV_UART5].Rd ) {
			UartBuf[DEV_UART5].Rd = (UartBuf[DEV_UART5].Rd + 1) % BUFFER_LEN;
		}			   		 
  } 
 #if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//�˳��ж�
#endif
} 

/****************************************************************************\
 Function:  UartWrite
 Descript:  д��������
 Input:	    ���ںţ����ݣ£գƣ�����
 Output:    �� 
 Return:    ʵ��д�����ݳ���
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




