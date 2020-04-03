#include "adc.h"
#include "sys.h"
#include "stm32f4xx.h"
#include <stdlib.h>
#include <string.h>
#include "usart.h"

static volatile uint8_t ADstatus;
static volatile uint8_t lastADstatus;
volatile uint32_t ADCSum=0;
static volatile uint32_t LastADCSum=0;

volatile u16 adcvalue[ADCVALUESIZE];



static void DMA2_Configuration(void) {
	DMA_InitTypeDef DMA_InitStructure;

	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&adcvalue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = ADCVALUESIZE;    //与通道数设置一致
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;            //DMA_FIFOMode_Disable        DMA_FIFOMode_Enable
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	DMA_Cmd(DMA2_Stream0, ENABLE);
}

static void ADC1_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure;

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;    //ADC_DataAlign_Right  ADC_DataAlign_Left　　设置数据右对齐
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_112Cycles);

}

void ADCInit(void)
{

	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_DMA2, ENABLE);//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	DMA2_Configuration();
	ADC1_Config();

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;        //ADC_DMAAccessMode_Disabled    ADC_DMAAccessMode_1
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_DMARequestAfterLastTransferCmd(ADC1,ENABLE);    //单通道使用

	ADC_Cmd(ADC1, ENABLE);

	ADC_DMACmd(ADC1, ENABLE);

	ADC_SoftwareStartConv(ADC1);
}


/************************************************************************************************************
** 函 数 名 : bubbleSort1
** 功能描述 : 冒泡排序
** 输    入 : 无
** 输    出 : 无
** 返    回 : 无
*************************************************************************************************************
*/
static void bubbleSort1(uint16_t arr[], uint16_t n) {
	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t tmp = 0;
	uint16_t flag = 0; 
	for (i = 0; i < n; ++i) {
		flag = 0;
		for (j = 0; j < n - 1 - i; ++j) {
			if (arr[j] < arr[j + 1]) {
				flag = 1;
				tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
			}
		}
		if (flag == 0) {
			break;
		}
	}	
}





uint32_t GetCpVol(void)
{
	uint16_t i;
	uint32_t sumval=0; 
	uint16_t sumval1 = 0;
	uint16_t datavalue[ADCVALUESIZE]={0};
	memcpy(datavalue, (char *)&adcvalue[0], sizeof(uint16_t)*ADCVALUESIZE);
	bubbleSort1(datavalue, ADCVALUESIZE);	
	for(i=10; i < (ADCVALUESIZE-10); i++){
		sumval += datavalue[i];
	}
	sumval = sumval / (ADCVALUESIZE-20);
	sumval1 = sumval;
	return sumval1;
}
/************************************************************************************************************
** 函 数 名 : Get_AdcStatus
** 功能描述 : 获取CP线  6 9 12V 电压 
** 输    入 : port ---预留   只支持A枪
** 输    出 : 无
** 返    回 : 4---12V   3 --- 9V  2 --- 6V   其他值为异常电压
*************************************************************************************************************
*/
uint8_t GetAdcStatus(void)
{
	return ADstatus;

}

/************************************************************************************************************
** 函 数 名 : ADCScan
** 功能描述 : 获取CP线  6 9 12V 电压 
** 输    入 : 无
** 输    出 : 无
** 返    回 : 无 
*************************************************************************************************************
*/
void ADCScan(void)
{
	
	int Result;
	static uint8_t cnt;
	LastADCSum = ADCSum;
	
	
	ADCSum = (GetCpVol() * 3714)/1000 + 180; ;  // ADCSum = Get_V(2) * 33 * 461 / 4096 +180   放大1000倍  毫伏 ，对12V无效 
	Result = (int)ADCSum - (int)LastADCSum;
	if(ADCSum < MIN6V){
		ADstatus = lastADstatus;
		cnt++;
		if(cnt == 6)
		ADstatus = ADC0V;	
	}else if(ADCSum < MAX6V){
		ADstatus = ADC6V;	//工作	
		cnt = 0;
		lastADstatus = ADstatus;
	}else if(ADCSum < MIN9V){
		if(abs(Result) >= ADTRANSITION){
			ADstatus = lastADstatus;
		}else{
			ADstatus = ADC0V;					
		}
	}else if(ADCSum < MAX9V){
		if(abs(Result) >= ADTRANSITION){
			ADstatus = lastADstatus;
		}else{
			ADstatus = ADC9V;
			cnt = 0;		
			lastADstatus = ADstatus;			
		}				
	}else if(ADCSum < MIN12V){
		if(abs(Result) >= ADTRANSITION){
			ADstatus = lastADstatus;
		}else{
			ADstatus = ADC0V;						
		}															
	}else if(ADCSum < MAX12V){
		ADstatus = ADC12V;																						
		cnt = 0;		
		lastADstatus = ADstatus;
	}else{
		lastADstatus = ADstatus;
		cnt++;
		if(cnt == 6)	
			ADstatus = ADC0V;																							
	}
	
}
