#include "pwm.h"
#include "sys.h"
#include "stm32f4xx.h"
#include "delay.h"
static uint8_t pwmflag = 0xFF;





/************************************************************************************************************
** �� �� �� : SetPwmOut
** �������� : ����ռ�ձ�
** ��    �� : Present   �ٷֱ�  (0- 100)
** ��    �� : ��
** ��    �� : ��
*************************************************************************************************************
*/
void SetPwmOut(uint8_t Present)
{
	if(pwmflag == Present)
		return;
	TIM_SetCompare2(TIM1, (Present)*10);
	pwmflag = Present;
}






void PwmInit(void)
{     
	GPIO_InitTypeDef  GPIO_InitStructure;        
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);    //TIM1ʱ��ʹ��  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);  
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1);  //GPIOE11����Ϊ��ʱ��1





	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;          //GPIOE11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       //���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //�ٶ�100MHz
	GPIO_Init(GPIOE,&GPIO_InitStructure);              //��ʼ��PA8,PA9,PA10,PA11
//	
//

	TIM_TimeBaseStructure.TIM_Prescaler = 168-1 ;            			 //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   	 //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;                       	 //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;        
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);          		 //��ʼ����ʱ��1
	
//		

//	//��ʼ��TIM1 Channel2   PWMģʽ        
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;     			//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 1000;			//���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  		//������Ե�
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);	//����Tָ���Ĳ�����ʼ������OC2


	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);	//ʹ��TIM1��CCR2�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM1,ENABLE);			//ARPEʹ�� 
	TIM_CtrlPWMOutputs(TIM1,ENABLE); 			//ʹ��TIM1��PWM���
	TIM_Cmd(TIM1,ENABLE);						//ʹ��TIM1  
}

