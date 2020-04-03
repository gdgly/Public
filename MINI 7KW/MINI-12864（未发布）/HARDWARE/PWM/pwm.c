#include "pwm.h"
#include "sys.h"
#include "stm32f4xx.h"
#include "delay.h"
static uint8_t pwmflag = 0xFF;





/************************************************************************************************************
** 函 数 名 : SetPwmOut
** 功能描述 : 设置占空比
** 输    入 : Present   百分比  (0- 100)
** 输    出 : 无
** 返    回 : 无
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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);    //TIM1时钟使能  
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);  
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1);  //GPIOE11复用为定时器1





	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;          //GPIOE11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       //复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   //不上下拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //速度100MHz
	GPIO_Init(GPIOE,&GPIO_InitStructure);              //初始化PA8,PA9,PA10,PA11
//	
//

	TIM_TimeBaseStructure.TIM_Prescaler = 168-1 ;            			 //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   	 //向上计数模式
	TIM_TimeBaseStructure.TIM_Period = 1000 - 1;                       	 //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;        
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);          		 //初始化定时器1
	
//		

//	//初始化TIM1 Channel2   PWM模式        
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;     			//选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  	//比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 1000;			//设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  		//输出极性低
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);	//根据T指定的参数初始化外设OC2


	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);	//使能TIM1在CCR2上的预装载寄存器
	TIM_ARRPreloadConfig(TIM1,ENABLE);			//ARPE使能 
	TIM_CtrlPWMOutputs(TIM1,ENABLE); 			//使能TIM1的PWM输出
	TIM_Cmd(TIM1,ENABLE);						//使能TIM1  
}

