#include "sys.h"
#include "pwm.h"
#include "adc.h"
#include "Para.h"
#include "Chgtask.h"
#include "Timer.h"
#include "includes.h"
#include "ElmTask.h"

static CHARGEINFO ChargeInfo;
static uint32_t Timer_MinCur;
static uint8_t StopCause;

static uint32_t DialIndicatorTimer;
static uint8_t DialIndicatorFlag;


/************************************************************************************************************
** 函 数 名 : SetDialIndicatorMode
** 功能描述 : 设置电表校表模式
** 输    入 : 无
** 输    出 : 无
** 返    回 : 无
*************************************************************************************************************
*/
void SetDialIndicatorMode(void)
{
	DialIndicatorTimer = TimerRead();
	DialIndicatorFlag = 1;
}

/************************************************************************************************************
** 函 数 名 : JudgeDialIndicator
** 功能描述 : 判断是否是电表校表
** 输    入 : 无
** 输    出 : 无
** 返    回 : 0：否   1 ==是
*************************************************************************************************************
*/
static uint8_t JudgeDialIndicator(void)
{
	if(DialIndicatorFlag == 0)
		return 0;
	if((TimerRead() - DialIndicatorTimer) > T1S * 2){
		DialIndicatorFlag = 0;
		return 0;
	}
	return 1;
}


/************************************************************************************************************
** 函 数 名 : InitChgGpio
** 功能描述 : 初始化充电相关管脚
** 输    入 : 无
** 输    出 : 无
** 返    回 : 1 ==急停按下   0 == 正常
*************************************************************************************************************
*/
void InitChgGpio(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  //上拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE,GPIO_Pin_9);						//PE10 输出低
	GPIO_ResetBits(GPIOE,GPIO_Pin_10);					//PE10 输出低
	
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;		//PE7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //下拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化PE7

}


/************************************************************************************************************
** 函 数 名 : TestStop
** 功能描述 : 急停检测
** 输    入 : 无
** 输    出 : 无
** 返    回 : 1 ==急停按下   0 == 正常
*************************************************************************************************************
*/
uint8_t TestStop(void)
{
	uint8_t Stop;
	static uint8_t LastStop = 0xFF;
	Stop = StopButton();   
	if(LastStop != Stop){
		OSTimeDlyHMSM(0, 0, 0, 10);
		Stop = StopButton();   
		LastStop = Stop;
	}
	if(Stop){
		return 1;
	}else{
		return 0;
	}
}


/************************************************************************************************************
** 函 数 名 : GetErrCur
** 功能描述 : 获取是否过流
** 输    入 : 无
** 输    出 : 无
** 返    回 : 1   过流   0  正常
*************************************************************************************************************
*/
 

static uint8_t GetErrCur(void)
{
	static uint32_t LimitCur;
	uint16_t pwm;
	
	if(LimitCur == 0){
		pwm = ParaGetPwm(0) * 100;
		if( 1000 <= pwm && pwm<= 8500){
			LimitCur = pwm * 6;
		}else if(pwm >8500 && pwm <= 9000 ){
			LimitCur = (pwm - 6400) * 25;
			if(LimitCur > 63000)
				LimitCur = 63000;
		}
		
		if(LimitCur <= 20000)
			LimitCur = LimitCur + 2000;
		else
			LimitCur = LimitCur * 110 /100;
	}
	
	if(ElmGetCurA() > LimitCur)
		return 1;
	else
		return 0;
}


	

/************************************************************************************************************
** 函 数 名 : StartOnebodyChg
** 功能描述 : 一体板启动充电函数
** 输    入 : 无
** 输    出 : 无
** 返    回 : 0 ==启动充电成功   1 == 启动充电失败
*************************************************************************************************************
*/
uint8_t StartCharge(void)
{
	uint32_t CntTimer;
	StopCause = CAUSE_USER_NORMAL_STOP;
	if(TestStop())
		return 1;	
	if((GetAdcStatus() == ADC9V || GetAdcStatus( ) == ADC6V)){	
		SetPwmOut(ParaGetPwm(0));
		CntTimer = TimerRead();			
		while( GetAdcStatus() != ADC6V){
			OSTimeDlyHMSM(0, 0, 0, 10);
			if((TimerRead() - CntTimer >= T1S*30) || (GetAdcStatus() == ADC12V) || (GetAdcStatus() == ADC0V)){
				StopCause = CAUSE_CP_FAULT;   //汽车3s内未打开 S2 开关
				SetPwmOut(100);
				return 1;
			}
		}
		OSTimeDlyHMSM(0, 0, 2, 0); //控制3S以内
		SWITCH_AC() = OPEN;
		SWITCH_ACN() = OPEN;
		Timer_MinCur = 0;
		
		ChargeInfo.ChargeState = 1;
		return 0;
	}else {
		return 1;
	}	
}



/************************************************************************************************************
** 函 数 名 : StopOnebodyChg
** 功能描述 : 一体板停止充电函数
** 输    入 : 无
** 输    出 : 无
** 返    回 : 1 == 停止充电成功   0 == 停止充电失败
*************************************************************************************************************
*/
uint8_t StopCharge(void )
{
	uint32_t CntTimer;
	SetPwmOut(100);
	CntTimer = TimerRead();	
	Timer_MinCur = 0;
	while(GetAdcStatus() == ADC6V){
		OSTimeDlyHMSM(0, 0, 0, 10);	
		if(TimerRead() - CntTimer >= T1S*3 ){							
			break;
		}
	}
	SWITCH_AC() = CLOSE;
	SWITCH_ACN() = CLOSE;
	ChargeInfo.ChargeState = 0;
	StopCause = CAUSE_USER_NORMAL_STOP;
	return 0;	
}

/************************************************************************************************************
** 函 数 名 : GetOnebodyStopCause
** 功能描述 : 一体式获取充电停止原因
** 输    入 : 无
** 输    出 : 无
** 返    回 : 充电停止原因
*************************************************************************************************************
*/
uint8_t GetStopCause(void)
{
	return StopCause;
}


/************************************************************************************************************
** 函 数 名 : GetChgInfo
** 功能描述 : 一体式获取充电信息
** 输    入 : 无
** 输    出 : 无
** 返    回 : CHARGEINFO  充电信息结构体
*************************************************************************************************************
*/
CHARGEINFO GetChgInfo(void)
{
	return ChargeInfo;
}




/************************************************************************************************************
** 函 数 名 : ChgOnebodyPro
** 功能描述 : 一体式充电处理函数
** 输    入 : port     0  ---- A枪      1 ---- B枪
** 输    出 : 无
** 返    回 : 无
*************************************************************************************************************
*/
void ChgOnebodyPro(uint8_t port)
{
	static uint32_t Timer_Cnt1 = 0;
	static uint32_t Timer_Cnt2 = 0;

	static uint8_t IsComplete = 0;
	static uint8_t Cnt9V = 0;
	uint8_t ChargeState;
	ADCScan();
	ChargeState = ChargeInfo.ChargeState;
	if((GetAdcStatus() == ADC9V || GetAdcStatus() == ADC6V) ){	
		ChargeInfo.GunSeatState = 1;
	}else{
		ChargeInfo.GunSeatState = 0;
	}	
	if(TestStop() == 0){   //急停故障
		ChargeInfo.ScramState = 0;
	}else{
		ChargeInfo.ScramState = 1;
		StopCause = CAUSE_SCRAMSTOP_FAULT;
		ChargeInfo.ChargeState = 0;
	}
	if(JudgeDialIndicator() == 0){
		if(ElmGetVolA() > 2420){  //过压
			ChargeInfo.HighVol = 1;
			StopCause = CAUSE_AC_OVERVOL_FAULT;
			ChargeInfo.ChargeState = 0;
		}else{
			ChargeInfo.HighVol = 0;
		}
		if(ElmGetVolA() < 1980){
			OSTimeDlyHMSM(0, 0, 0, 100);
			if(ElmGetVolA() < 1980){
				ChargeInfo.LowVol = 1;
				StopCause = CAUSE_AC_LESSVOL_FAULT;
				ChargeInfo.ChargeState = 0;
			}
		}else{
			ChargeInfo.LowVol = 0;
		}	
	}else{
		ChargeInfo.HighVol = 0;
		ChargeInfo.LowVol = 0;
	}

	ChargeInfo.CurA = ElmGetCurA();
	ChargeInfo.VolA = ElmGetVolA();
	if( GetErrCur()){  //判断过流
		if(Timer_Cnt1 == 0)
			Timer_Cnt1 = TimerRead();
		if((TimerRead() - Timer_Cnt1) >= T1S *5){
			StopCause = CAUSE_AC_OVERCUR_FAULT;    //过流
			if(JudgeDialIndicator() == 0){
					ChargeInfo.ChargeState = 0;
			}
		}
	}else{
		Timer_Cnt1 = 0;
	}

	if( ChargeInfo.ChargeState ){    //充电时判断
		if(ElmGetCurA() < 2000){
			if( Timer_MinCur == 0)
				Timer_MinCur = TimerRead();
			if((TimerRead() - Timer_MinCur) >= T1M *2){
				Timer_MinCur = 0;
				StopCause = CAUSE_AC_LESSCUR_FAULT;    //欠流
				if(JudgeDialIndicator() == 0){
					ChargeInfo.ChargeState = 0;
				}
				
			}
		}else{
				Timer_MinCur = 0;
		}
	 if( ElmGetCommState()){   
			StopCause = CAUSE_ELM_FAULT;         //电表异常
			ChargeInfo.ChargeState = 0;
		}
		if( GetAdcStatus() == ADC0V || GetAdcStatus() == ADC12V){    // CP异常
			SetPwmOut(100);
			OSTimeDlyHMSM(0, 0, 0, 20);
			SWITCH_AC() = CLOSE;
			SWITCH_ACN() = CLOSE;
			StopCause = CAUSE_CP_FAULT;
			ChargeInfo.ChargeState = 0;
		}else if(GetAdcStatus() == ADC6V){		   //充电中CP跳到 6V
			if(IsComplete){
				IsComplete = 0;
				OSTimeDlyHMSM(0, 0, 2, 0); //控制3S以内
				Timer_MinCur = 0;
				SWITCH_AC() = OPEN;
				SWITCH_ACN() = OPEN;			
			}
		}else if(GetAdcStatus() == ADC9V){		   //充电中CP跳到 9V		
			Timer_MinCur = TimerRead();
			if(IsComplete == 0){
				SWITCH_AC() = CLOSE;
				SWITCH_ACN() = CLOSE;
				IsComplete = 1;
				Cnt9V++;
				if(Cnt9V >= 4){
					StopCause = CAUSE_PAUSE_TIMES_FAULT;
					ChargeInfo.ChargeState = 0;
				}
				Timer_Cnt2 = TimerRead(); 
			}else{
				if(TimerRead() - Timer_Cnt2 >= T1M ){
					Timer_Cnt2 = 0;
					ChargeInfo.ChargeState = 0;
					StopCause = CAUSE_SUSPEND_TIMEOUT; //充电过程中跳变9V超过1分钟认为 充电桩充电中暂停超时 
				}
			}
		}
	}
	if(ChargeState &&(ChargeInfo.ChargeState == 0)){
		SetPwmOut(100);
		Cnt9V = 0;
		OSTimeDlyHMSM(0, 0, 0, 25);		
		SWITCH_AC() = CLOSE;
		SWITCH_ACN() = CLOSE;
	}
}


void ChargeTask(void * pdata)
{
	while(1) {
		ChgOnebodyPro(0);
		OSTimeDlyHMSM (0, 0, 0, 15);
		
	}
}

