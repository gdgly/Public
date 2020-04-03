

#include "sys.h"
#include "led.h"
#include "rtc.h"
#include "timer.h"
#include "Record.h"
#include "MainTask.h"
#include "usart.h"
#include "Para.h"
#include "CardTask.h"
#include "Chgtask.h"
#include "Cfg.h"
#include "Display.h"
#include "ElmTask.h"
#include "stdio.h"
#include "string.h"
#include "includes.h"
#include "stmflash.h"
#include "Protocol.h"
#include "EthUdp.h"


 MAININFO MainInfo;

static uint8_t LEDState;

static uint8_t SetMainChargeFlag;//1启动 2停止
static uint8_t PlatformMode;//平台启动时的模式 0充满 1按电量 2按时间 3按金额
static uint32_t PlatformPara;//平台启动时的参数 按充满时精度为0.01元 按电量时精度为0.01kWh 按时间时精度为秒 按金额时精度为0.01元
static uint8_t PlatformCardOrDiscernNumber[1][32]; //平台启动时的充电卡号/用户识别号
static uint8_t PlatformUserID[2];//用户ID

static uint8_t OneGunState;//单枪和双枪状态

static uint8_t LifeTimeExpireFlag;//使用期限到期标志 0未到期 1到期
static	uint32_t SerialNum;       //离线卡 交易流水号




static uint8_t UpdataInFlag; //更新中标志

static uint8_t ReportChargeData;    //上报充电记录标志

static uint32_t TimerCntRun; 

static uint8_t StartingChg;   //充电启动中标志

/************************************************************************************************************
** 函 数 名 : GetStartType
** 功能描述 : 获取启动方式
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	STARTCHARGETYPE_OFFLINECARD STARTCHARGETYPE_ONLINECARD STARTCHARGETYPE_PLATFORM
*************************************************************************************************************
*/
uint8_t GetStartType(uint8_t port)
{
	return MainInfo.ChgDat.StartType;
}

/************************************************************************************************************
** 函 数 名 : GetLifeTime
** 功能描述 : 获取是否租赁到期
** 输    入 : 无
** 输    出 : 无
** 返    回 :	1 到期   0 正常
*************************************************************************************************************
*/
uint8_t GetLifeTime(void)
{
	return LifeTimeExpireFlag;
}

/************************************************************************************************************
** 函 数 名 : GetStartingChg
** 功能描述 : 获取是否是启动中
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	0 不是启动中  1  启动中
*************************************************************************************************************
*/
uint8_t GetStartingChg(uint8_t port)
{
	return StartingChg;
}

/************************************************************************************************************
** 函 数 名 : GetStartCardID
** 功能描述 : 获取启动卡ID
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	卡ID
*************************************************************************************************************
*/
uint32_t GetStartCardID(uint8_t port)
{
	return MainInfo.ChgDat.StartCardID;
}

/************************************************************************************************************
** 函 数 名 : GetStartMode
** 功能描述 : 获取启动模式
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	0充满 1按电量 2按时间 3按金额
*************************************************************************************************************
*/
uint8_t GetStartMode(uint8_t port)
{
	return MainInfo.ChgDat.Mode;
}

/************************************************************************************************************
** 函 数 名 : GetStartPara
** 功能描述 : 获取启动参数
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	
*************************************************************************************************************
*/
uint32_t GetStartPara(uint8_t port)
{
	return MainInfo.ChgDat.StartCardMoney; //自动充满，别的不支持
}

/************************************************************************************************************
** 函 数 名 : GetVoltage
** 功能描述 : 获取电压值
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : A,B,C相电压
** 返    回 :	无
*************************************************************************************************************
*/
void GetVoltage(uint8_t port, uint32_t *VolA, uint32_t *VolB, uint32_t *VolC)
{
	*VolA = MainInfo.GunInfo.VoltageA;
	*VolB = 0;
	*VolC = 0;
}

/************************************************************************************************************
** 函 数 名 : GetCurrent
** 功能描述 : 获取电流值
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : A,B,C相电流
** 返    回 :	无
*************************************************************************************************************
*/
void GetCurrent(uint8_t port, uint32_t *CurA, uint32_t *CurB, uint32_t *CurC)
{
	*CurA = MainInfo.GunInfo.CurrentA;
	*CurB = 0;
	*CurC = 0;
}

/************************************************************************************************************
** 函 数 名 : GetStartMeterEnergy
** 功能描述 : 获取启动时电表电量
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	电量 0.01KWH
*************************************************************************************************************
*/
uint32_t GetStartMeterEnergy(uint8_t port)
{
	return (uint32_t) MainInfo.ChgDat.StartMeterEnergy;
}

/************************************************************************************************************
** 函 数 名 : GetStopMeterEnergy
** 功能描述 : 获取停止时电表电量
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	电量 0.01KWH
*************************************************************************************************************
*/
uint32_t GetStopMeterEnergy(uint8_t port)
{
	return (uint32_t)MainInfo.ChgDat.StopMeterEnergy;
}

/************************************************************************************************************
** 函 数 名 : GetSumEnergy
** 功能描述 : 获取累计充电电量
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	电量 0.01KWH
*************************************************************************************************************
*/
uint32_t GetSumEnergy(uint8_t port)
{
	return MainInfo.ChgDat.SumEnergy;
}

/************************************************************************************************************
** 函 数 名 : GetSumMoney
** 功能描述 : 获取累计充电金额
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	金额 0.01元
*************************************************************************************************************
*/
uint32_t GetSumMoney(uint8_t port)
{
	return MainInfo.ChgDat.SumMoney;
}

/************************************************************************************************************
** 函 数 名 : GetPeriodEnergy
** 功能描述 : 获取时段电量
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	电量 0.01KWH
*************************************************************************************************************
*/
uint16_t GetPeriodEnergy(uint8_t port, uint8_t Period)
{
	return MainInfo.ChgDat.PeriodEnergy[Period];
}

/************************************************************************************************************
** 函 数 名 : GetSumTime
** 功能描述 : 获取累计时间
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 : 时间 
*************************************************************************************************************
*/
uint32_t GetSumTime(uint8_t port)
{
	return MainInfo.ChgDat.SumTime;
}


/************************************************************************************************************
** 函 数 名 : JudgeLifeTime
** 功能描述 : 判断使用期限
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
static void JudgeLifeTime(void)
{
	uint32_t LimitHours,  RunHours;
	LimitHours = ParaGetLimitHour();
	
	LifeTimeExpireFlag = 0;
	if( LimitHours ){	
		RunHours = ParaGetRunHour();
		if((TimerRead()- TimerCntRun) >= T1H){
			TimerCntRun = TimerRead();
			RunHours++;
			ParaSavaRunHour(RunHours);
		}
		if(RunHours >= LimitHours )
			LifeTimeExpireFlag = 1;
	}
}



/************************************************************************************************************
** 函 数 名 : GetStartDateTime
** 功能描述 : 获取启动时日期时间
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 日期时间
** 返    回 : 无
*************************************************************************************************************
*/
void GetStartDateTime(uint8_t port, uint8_t *datetime)
{
	memcpy(datetime, MainInfo.ChgDat.StartDateTime, 6);
}

/************************************************************************************************************
** 函 数 名 : GetStopDateTime
** 功能描述 : 获取停止时日期时间
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 日期时间
** 返    回 : 无
*************************************************************************************************************
*/
void GetStopDateTime(uint8_t port, uint8_t *datetime)
{
	memcpy(datetime, MainInfo.ChgDat.StopDateTime, 6);
}

/************************************************************************************************************
** 函 数 名 : GetStartCardMoney
** 功能描述 : 获取启动时卡余额
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 : 余额 0.01元
*************************************************************************************************************
*/
uint32_t GetStartCardMoney(uint8_t port)
{
	return MainInfo.ChgDat.StartCardMoney;
}



/************************************************************************************************************
** 函 数 名 : SetLEDState
** 功能描述 : 设置灯板显示状态
** 输    入 : port state 0空闲 1充电 2故障
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
static void SetLEDState(uint8_t port, uint8_t state)
{
	LEDState = state;
}

/************************************************************************************************************
** 函 数 名 : Beep
** 功能描述 : 蜂鸣器响程序
** 输    入 : ms 响持续时间 毫秒
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
static void Beep(uint32_t ms)
{

}

/************************************************************************************************************
** 函 数 名 : JudgePeriod
** 功能描述 : 判断时间函数
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......    time 0-47
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
static uint32_t CalPeriodMoney(uint8_t port, uint8_t time)
{
	uint32_t money = 0,i,starttime, stoptime,caltime;
	uint8_t starthour, startmin, stophour, stopmin;
	
	for (i = 0; i < 12; i++){
		ParaGetFeilvTime(port, i, &starthour, &startmin, &stophour, &stopmin);
		if ((starthour != stophour) || (startmin != stopmin)){
			starttime = starthour * 60 + startmin;
			stoptime = stophour * 60 + stopmin;
			caltime = time * 30;
			if ((caltime >= starttime) && (caltime < stoptime)){//前闭后开
				money = ParaGetFeilvMoney(port, i);
				break; 
			}
		}
	}
	return money;
}

/************************************************************************************************************
** 函 数 名 : GetOnlineCardInfo
** 功能描述 : 获取在线卡信息
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪...... CardID 卡号 CardBalance 余额
** 输    出 : 无
** 返    回 :	获取成功1 获取失败0
*************************************************************************************************************
*/
static uint8_t GetOnlineCardInfo(uint8_t port, uint32_t CardID, uint32_t *CardBalance)
{
	uint8_t verifyonlineret;
	uint8_t TmpBuf[48];

	//提示正在验证中，请稍等
	DispInVerify();
	//滴声提示已响应操作
	Beep(100);
	//核实卡信息
	if ((verifyonlineret = VerifyOnlineCardInfo(port, CardID, CardBalance)) == 0){
		return 1;
	} else {
		//滴声提示已响应操作
		Beep(100);
		//提示验证失败
		if (verifyonlineret == 0xff)
			sprintf((char *)TmpBuf, "响应超时");
		else if (verifyonlineret == 2)
			sprintf((char *)TmpBuf, "余额不足");
		else if (verifyonlineret == 5)
			sprintf((char *)TmpBuf, "卡号不存在");
		else if (verifyonlineret == 6)
			sprintf((char *)TmpBuf, "已挂失");
		else if (verifyonlineret == 7)
			sprintf((char *)TmpBuf, "已销卡");
		else if (verifyonlineret == 9)
			sprintf((char *)TmpBuf, "正在其他桩充电");
		else
			sprintf((char *)TmpBuf, "其他原因");
		DispVerifyFailure(verifyonlineret);
		OSTimeDlyHMSM (0, 0, 5, 0);
		return 0;
	}
}





/************************************************************************************************************
** 函 数 名 : InitStartChgDate
** 功能描述 : 初始化开始充电数据
** 输    入 : Maininfo  充电信息结构体   
**  				:	Mode  0离线卡  3在线卡   1后台   port ==0 A枪   ==1B枪
**  				:	CardId   卡ID  Money == StartCardMoney 
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
static void InitStartChgDate(MAININFO* Info, uint8_t Mode, uint32_t CardId, uint32_t Money)
{
	Rtc datetime;
	ReportChargeData = 0;
	memset(&Info->ChgDat, 0, sizeof(CHARGINGDATA));
	memset(&Info->GunInfo, 0, sizeof(GUNINFO));
	RtcRead(&datetime);
	Info->ChgDat.StartDateTime[0] = datetime.year % 2000;
	Info->ChgDat.StartDateTime[1] = datetime.month;
	Info->ChgDat.StartDateTime[2] = datetime.day;
	Info->ChgDat.StartDateTime[3] = datetime.hour;
	Info->ChgDat.StartDateTime[4] = datetime.min;
	Info->ChgDat.StartDateTime[5] = datetime.sec;
	memcpy(Info->ChgDat.StopDateTime, Info->ChgDat.StartDateTime, 6);
	Info->ChgDat.StartMeterEnergy = ElmGetTotalElc(1);
	Info->ChgDat.StopMeterEnergy = Info->ChgDat.StartMeterEnergy;
	Info->GunInfo.StartTimer = TimerRead();
	Info->GunInfo.StopTimer = Info->GunInfo.StartTimer;
	Info->ChgDat.StartType = Mode;
	Info->ChgDat.StartCardID = CardId;
	Info->ChgDat.Mode = 0;
	Info->ChgDat.StartCardMoney = Money;
	printf("StartTimer：%d\r\n", Info->GunInfo.StartTimer);
	if(Mode == 0){   //  离线卡
		Info->ChgDat.SerialNum = SerialNum;		
	}else if(Mode == 3){     //  在线卡			
		Info->ChgDat.SerialNum = 0;	
	}else if(Mode == 1){   // 平台
		Info->ChgDat.Mode = PlatformMode;
		Info->ChgDat.StartCardMoney = Money;
//		memcpy(Info->ChgDat.CardOrDiscernNumber, PlatformCardOrDiscernNumber[port], 32);
		Info->ChgDat.StartCardID = PlatformUserID[0] | PlatformUserID[1] << 8;
		Info->ChgDat.SerialNum = 0;
	}
	StartingChg = TRUE;
}


/************************************************************************************************************
** 函 数 名 : UpdateChargeData
** 功能描述 : 更新充电时的相关数据
** 输    入 : ChgData  MAININFO结构体指针   
**  				:	info  CHARGEINFO结构体
**  				:	port ==0 A枪   ==1B枪
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
static void UpdateChargeData(MAININFO* ChgData, CHARGEINFO info, uint8_t port)
{
	uint32_t data32;
	Rtc datetime;
	uint32_t UpdateMoney = 0;
	uint8_t period, i;
	
	if( ChgData->ChgDat.StartType ==  STARTCHARGETYPE_PLATFORM ) {
		UpdateMoney = UpdateMoneyJudge(port, (ChgData->ChgDat.StartCardID | (ChgData->ChgDat.StartCardID << 8)),  ChgData->ChgDat.StartCardMoney);
		if( UpdateMoney ){
			if(ChgData->ChgDat.Mode == 3){ //按金额
				ChgData->ChgDat.Para = UpdateMoney;
			}
			ChgData->ChgDat.StartCardMoney = UpdateMoney;
			InsertChargingRecord( &ChgData->ChgDat );
		}
	}
	//计算充电中数据
	ChgData->GunInfo.VoltageA = ElmGetVolA();

	ChgData->GunInfo.CurrentA = ElmGetCurA();
	data32 = ElmGetTotalElc(0);
	if (data32 != ChgData->ChgDat.StopMeterEnergy){
		RtcRead(&datetime);
		period = (datetime.hour * 60 + datetime.min) / 30;
		ChgData->ChgDat.PeriodEnergy[period] += data32 - ChgData->ChgDat.StopMeterEnergy;
	}
	ChgData->ChgDat.StopMeterEnergy = data32;
	ChgData->GunInfo.StopTimer = TimerRead();
	ChgData->ChgDat.SumTime = (ChgData->GunInfo.StopTimer - ChgData->GunInfo.StartTimer) / T1S;
	data32 = 0;
	for (i = 0; i <  48; i++)
		data32 += ChgData->ChgDat.PeriodEnergy[i] * CalPeriodMoney(port, i);
	ChgData->ChgDat.SumMoney = (data32 + 90000)/ 100000;//始终加0.009元，补后台的服务费和充电费分开的问题
	RtcRead(&datetime);
	ChgData->ChgDat.StopDateTime[0] = datetime.year % 2000;
	ChgData->ChgDat.StopDateTime[1] = datetime.month;
	ChgData->ChgDat.StopDateTime[2] = datetime.day;
	ChgData->ChgDat.StopDateTime[3] = datetime.hour;
	ChgData->ChgDat.StopDateTime[4] = datetime.min;
	ChgData->ChgDat.StopDateTime[5] = datetime.sec;
	if ( ChgData->ChgDat.SumEnergy != (ChgData->ChgDat.StopMeterEnergy - ChgData->ChgDat.StartMeterEnergy)){
		ChgData->ChgDat.SumEnergy = ChgData->ChgDat.StopMeterEnergy - ChgData->ChgDat.StartMeterEnergy;
		InsertChargingRecord(&ChgData->ChgDat);
	}
}









///************************************************************************************************************
//** 函 数 名 : ShowMenu
//** 功能描述 : 显示 菜单栏相关信息
//** 输    入 : 无
//** 输    出 : 无
//** 返    回 :	无
//*************************************************************************************************************
//*/
//static void ShowMenu(void)
//{
//	uint8_t timebuf[32] = {0};
//	uint8_t linebuf[32] = {0};
//	Rtc DateAndTim;
//	
//	RtcRead(&DateAndTim);
//	sprintf((char *)timebuf,"%02d:%02d",DateAndTim.hour,DateAndTim.min);

//	if(ParaGetChgGunNumber()== 1 ){
//		if (GetOnlineState(0) == 1)
//			sprintf((char *)linebuf, "在线");
//		else
//			sprintf((char *)linebuf, "   ");
//	}else{
//		if((GetOnlineState(0) == 1) && (GetOnlineState(1) == 1)){
//			sprintf((char *)linebuf, "在线");
//		} else if( GetOnlineState(0) == 1){
//			sprintf((char *)linebuf, "A在线");
//		}	else if( GetOnlineState(1) == 1){
//			sprintf((char *)linebuf, "B在线");
//		}else {
//			sprintf((char *)linebuf, "   ");
//		}		
//	}
//	DispMenuInfo(timebuf, linebuf );		
//}


/************************************************************************************************************
** 函 数 名 : UnLockCardPro
** 功能描述 : 处理锁卡相关流程
** 输    入 : Cardid   卡ID   Money  写的金额
** 输    出 : 无
** 返    回 :	1 解锁写金额成功  0 失败
*************************************************************************************************************
*/
static uint8_t UnLockCardPro(uint32_t cardid, uint32_t Money)
{
	uint32_t Timer;
	
	if (CardSetMoneyAndUnlock(cardid, Money)){//解锁卡并扣费成功
		return 1;
	}
	//显示操作中界面		
	DispOperationInfo();
	Timer = TimerRead();
	while((TimerRead() - Timer) < T1S * 10){
		OSTimeDlyHMSM (0, 0, 0, 20);
		if (CardSetMoneyAndUnlock(cardid, Money)){//解锁卡并扣费成功
			while((TimerRead() - Timer) < T1S * 2){
				OSTimeDlyHMSM (0, 0, 0, 20);
			}
			return 1;
		}		
	}
	return 0;
}




/************************************************************************************************************
** 函 数 名 : LockedCardHandle
** 功能描述 : 处理锁卡相关操作
** 输    入 : cardid   卡ID   money  写的金额
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
static void LockedCardHandle(uint32_t cardid, uint32_t money)
{
	uint8_t TmpBuf[257],PileNumber[32];
	uint8_t ret;
	uint8_t RecordNumber;
	uint32_t i,SerialNum,index;
	CHARGINGDATA chargingdata;

	memset(TmpBuf, 0, sizeof(TmpBuf));
	memset(PileNumber, 0, sizeof(PileNumber));
	ParaGetPileNo(0, TmpBuf);
	for (i = 0; i < 4; i++)
		PileNumber[i] = TmpBuf[strlen((const char *)TmpBuf) - 4 + i];
	memset(TmpBuf, 0, sizeof(TmpBuf));
	CardGetPileNumber(TmpBuf);
	if ((memcmp(TmpBuf, PileNumber, 4) == 0) || (FindLockCardRecord(cardid, &SerialNum) == 1)){//该卡被本桩锁住
		ret = 0;
		if (FindLockCardRecord(cardid, &SerialNum) == 1){
			index = GetChargeRecordIndex(0);
			RecordNumber = GetChargeRecordNumber(0);
			for (i = 0; i < RecordNumber; i++){//从最近100条记录里面查找
				if (FindChargeRecord(index,&chargingdata) == 1){
					if (chargingdata.SerialNum == SerialNum){
						ret = 1;
						break;
					}
				}
				if (index == 0)
					index = RECORD_TOTAL_NUM-1;
				else
					index --;
			}
		}
		if (ret == 1){//记录查询成功
			if (money > chargingdata.SumMoney)
				money = money - chargingdata.SumMoney;
			else
				money = 0;		
			
			if(UnLockCardPro(cardid, money)){
				Beep(100);
				DeleteLockCardRecord(cardid);
				OSTimeDlyHMSM (0, 0, 2, 0);
				DispAccount1(chargingdata.SumEnergy, chargingdata.SumMoney, money, chargingdata.SumTime, chargingdata.StopDateTime);
				OSTimeDlyHMSM (0, 0, 3, 0);
				DispAccount2(chargingdata.SumEnergy, chargingdata.SumMoney, money, chargingdata.SumTime, chargingdata.StopDateTime);
				OSTimeDlyHMSM (0, 0, 3, 0);
			}else{
				Beep(100);
				DispUnLockFailureInfo(UNLOCKCARDFAILCODE_WRITECARD);
				OSTimeDlyHMSM (0, 0, 5, 0);	
			}
		} else {//找不到锁卡记录
			Beep(100);
			DispUnLockFailureInfo(UNLOCKCARDFAILCODE_NORECORD);
			UnLockCardPro(cardid, 10000);
			OSTimeDlyHMSM (0, 0, 5, 0);
		}
	} else {//提示其他桩锁住
		Beep(100);	
		DispCardLockInfo(TmpBuf);
		UnLockCardPro(cardid, 10000);
		OSTimeDlyHMSM (0, 0, 5, 0);
	}
}




/************************************************************************************************************
** 函 数 名 : LockCardPro
** 功能描述 : 处理锁卡相关流程
** 输    入 : Cardid   卡ID
** 输    出 : 无
** 返    回 :	1 锁卡成功  0 失败
*************************************************************************************************************
*/
static uint8_t LockCardPro(uint32_t Cardid)
{
	uint8_t PileNumber[32];
	uint8_t TmpBuf[56];
	uint8_t i;
	uint32_t Timer;
	Timer = TimerRead();
	memset(TmpBuf, 0, sizeof(TmpBuf));
	memset(PileNumber, 0, sizeof(PileNumber));
	ParaGetPileNo(0, TmpBuf);
	for (i = 0; i < 4; i++)
		PileNumber[i] = TmpBuf[strlen((const char *)TmpBuf) - 4 + i];
	if( CardSetPileNumberAndLock(Cardid, PileNumber) == 1){
		SerialNum = InsertLockCardRecord(Cardid );			//存储锁卡记录		
		return 1;
	}
	//显示操作中界面		
	DispOperationInfo();
	while((TimerRead() - Timer) < T1S * 10){
		OSTimeDlyHMSM (0, 0, 0, 20);
		if( CardSetPileNumberAndLock(Cardid, PileNumber) == 1){
			SerialNum = InsertLockCardRecord(Cardid );			//存储锁卡记录	
			while((TimerRead() - Timer) < T1S * 2){
				OSTimeDlyHMSM (0, 0, 0, 20);
			}
			return 1;
		}
	}
	return 0;
}




/************************************************************************************************************
** 函 数 名 : ConditionStop
** 功能描述 : 判断充电是否自动停止
** 输    入 : MainInf  充电相关结构体   port  0 ==A枪    1 ==B枪 
** 输    出 : 无
** 返    回 :	1 停止  0 正常
*************************************************************************************************************
*/
static uint8_t ConditionStop( MAININFO* MainInf )
{
	if ((MainInf->ChgDat.SumMoney + MINIMUM_CHARGE_MONEY) > MainInf->ChgDat.StartCardMoney){//按金额
		MainInf->ChgDat.StopCause = CAUSE_MONEY_FAULT;
		MainInf->ChgDat.SumMoney = MainInf->ChgDat.StartCardMoney;
		return 1;	
//	}else if ((MainInf->ChgDat.Mode == 1) && (MainInf->ChgDat.SumEnergy >= MainInf->ChgDat.Para)){//按电量
//		MainInf->ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;
//		return 1;
//	}else if ((MainInf->ChgDat.Mode == 2) && (MainInf->ChgDat.SumTime >= MainInf->ChgDat.Para)){//按时间
//		MainInf->ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;
//		return 1;
//	}else if ((MainInf->ChgDat.Mode == 3) && (MainInf->ChgDat.SumMoney >= MainInf->ChgDat.Para)){//按金额
//		MainInf->ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;
//		return 1;
//	
	}else if (ElmGetCommState() == 1){
		MainInf->ChgDat.StopCause = CAUSE_ELM_FAULT;
		return 1;
	}else
		return 0;
}



/************************************************************************************************************
** 函 数 名 : OneGunHandle
** 功能描述 : 单枪处理函数
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
static void OneGunHandle(void)
{
	CHARGEINFO info;
	uint8_t TmpBuf[257],*pTmpBuf,PileNumber[32];
	static uint8_t cardinflag;
	uint8_t SwingOfflineCardFlag = 0;//刷离线卡标志 0未刷卡 1刷卡
	uint8_t SwingOnlineCardFlag = 0;//刷在线卡标志 0未刷卡 1刷卡
	uint8_t CardLockState;//卡锁状态 1正常 2锁卡
	uint32_t CardID;//卡号
	uint32_t CardBalance;//卡余额 2位小数
	uint8_t ret;
	uint32_t money;
	uint16_t Updata;
	StartingChg = FALSE;
	//获取卡信息
	if (CardGetState() == 1){
		if (cardinflag != 1){
			cardinflag = 1;
			CardID = CardGetId();
			CardBalance = CardGetMoney();
			CardLockState = CardGetLockState();
			if (CardGetType() == 1)
				SwingOfflineCardFlag = 1;
			else
				SwingOnlineCardFlag = 1;
		}
	} else {
		cardinflag = 0;
	}


	//计算使用期限
	JudgeLifeTime();
	DisHeadInfo(GetOnlineState(0));
	switch(OneGunState){
		case A_IDLE://空闲---------------------------------------------------------------------------------------------------------------------------------
			info = GetChgInfo();
			Updata = GetUpdataPre();
			if(( Updata>> 8) == 1){   //升级中
				if(UpdataInFlag == 0)
					Beep(100);
				UpdataInFlag = 1;
				DispUpdataInfo(Updata&0xFF, 1);
			}else if(UpdataInFlag){
				 //显示升级失败
				Beep(100);
				UpdataInFlag = 0;
				DispUpdataInfo(Updata&0xFF,3);
				OSTimeDlyHMSM (0, 0, 5, 0);	
			}else if ((info.ScramState == 1) || (ElmGetCommState() == 1) || (LifeTimeExpireFlag == 1) || (info.HighVol == 1)|| (info.LowVol == 1)){//充电桩故障
				SetLEDState(0, 2);
				memset(TmpBuf, 0, sizeof(TmpBuf));
				pTmpBuf = TmpBuf;
				if (info.ScramState == 1)
					pTmpBuf = pTmpBuf + sprintf((char *)pTmpBuf, "%02d ", FAULTCODE_SCRAM);
				if (ElmGetCommState() == 1)
					pTmpBuf = pTmpBuf + sprintf((char *)pTmpBuf, "%02d ", FAULTCODE_METER_COMM);
				if (LifeTimeExpireFlag == 1)
					pTmpBuf = pTmpBuf + sprintf((char *)pTmpBuf, "%02d ", FAULTCODE_EXPIRE_COMM);
				if (info.HighVol == 1)
					pTmpBuf = pTmpBuf + sprintf((char *)pTmpBuf, "%02d ", FAULTCODE_HIGHVOL);
				if (info.LowVol == 1)
					pTmpBuf = pTmpBuf + sprintf((char *)pTmpBuf, "%02d ", FAULTCODE_LOWVOL);			
				DispDeviceFault(TmpBuf);
				if (SwingOfflineCardFlag == 1){   //A待机B待机刷离线卡
					SwingOfflineCardFlag = 0;
					if (CardLockState == 1){//正常	
						//滴声提示已响应操作
						Beep(100);
						DispStartFailureInfo();
						OSTimeDlyHMSM (0, 0, 5, 0);	
					}
					if (CardLockState == 2){//灰锁卡
						LockedCardHandle(CardID, CardBalance);
					}
				}
				if(SwingOnlineCardFlag == 1){
					SwingOnlineCardFlag = 0;
					//滴声提示已响应操作
					Beep(100);
					DispStartFailureInfo();
					OSTimeDlyHMSM (0, 0, 5, 0);	
				}			
			} else {//充电桩正常
				//更新指示灯显示状态
				SetLEDState(0, 0);
				
				//更新显示屏数据
				ParaGetPileNo(0, PileNumber);
				strcat((char* )PileNumber,"A");
				DispAIdleInfo(info.GunSeatState, PileNumber);	
					//判断并处理刷离线卡操作
				if (SwingOfflineCardFlag == 1){//刷离线卡
					SwingOfflineCardFlag = 0;		
					if (CardLockState == 1){//正常卡
						if (info.GunSeatState == 0){//未插枪
							//滴声提示已响应操作
							Beep(100);						
							//提示未插枪
							DispInsertGunInfo();
							OSTimeDlyHMSM (0, 0, 5, 0);
							
						} else {						
							if( LockCardPro(CardID)){
								//滴声提示已响应操作
								Beep(100);
								//初始化枪数据								
								InitStartChgDate( &MainInfo, STARTCHARGETYPE_OFFLINECARD, CardID, CardBalance);
								InsertChargingRecord( &MainInfo.ChgDat);	
						
								//显示卡余额
								DispRemainMoneyInfo(CardBalance);
								OSTimeDlyHMSM (0, 0, 2, 0);	
								
								//显示启动中界面
								DispStartChgInfo();
								OSTimeDlyHMSM (0, 0, 0, 500);
								
								//启动界面
								ret = StartCharge();
								if (ret == 0){//启动成功
									OneGunState = A_CHARGE;
								} else {
									MainInfo.ChgDat.StopCause = GetStopCause();//GetCCBStopCause(0);
									//生成充电记录,并删除充电过程数据
									ReportChargeData = 1;
									InsertChargeRecord( &MainInfo.ChgDat);
									DeleteChargingRecord(0);
									OneGunState = A_STOP;
									DispStartFailureInfo(); //显示启动失败
									OSTimeDlyHMSM (0, 0, 2, 0);
								}
							}else{
								DispStartFailureInfo();
								OSTimeDlyHMSM (0, 0, 5, 0);
							}		
						}
					}
					if (CardLockState == 2){//灰锁卡
						LockedCardHandle(CardID, CardBalance);
					}
				}	else if (SwingOnlineCardFlag == 1){//刷在线卡
					SwingOnlineCardFlag = 0;	
					if (info.GunSeatState == 0){//未插枪
						//滴声提示已响应操作
						Beep(100);
						//提示未插枪
						DispInsertGunInfo();
						OSTimeDlyHMSM (0, 0, 5, 0);
						
					} else {
						//获取在线卡信息
						if (GetOnlineCardInfo(0, CardID, &CardBalance) == 1){
							//初始化枪数据
							InitStartChgDate( &MainInfo, STARTCHARGETYPE_ONLINECARD, CardID, CardBalance);
							InsertChargingRecord(&MainInfo.ChgDat);						
							//显示卡余额
							DispRemainMoneyInfo(CardBalance );
							OSTimeDlyHMSM (0, 0, 2, 0);							
							//先提示启动充电中界面
							DispStartChgInfo();
							OSTimeDlyHMSM (0, 0, 0, 500);						
							//启动充电
							ret = StartCharge();
							if (ret == 0){//启动成功
								OneGunState = A_CHARGE;
							} else {
								MainInfo.ChgDat.StopCause = GetStopCause();//GetCCBStopCause(0);
								//生成充电记录,并删除充电过程数据
								ReportChargeData = 1;
								InsertChargeRecord( &MainInfo.ChgDat);
								DeleteChargingRecord(0);
								OneGunState = A_STOP;
							}
						}else {
						
						}
					}
				} else if (SetMainChargeFlag == 1) {//后台启动     				//判断并处理后台操作
					SetMainChargeFlag = 0;				
					if (info.GunSeatState == 0){//未插枪
						//滴声提示已响应操作
						Beep(100);
						//提示未插枪
						DispInsertGunInfo();
						OSTimeDlyHMSM (0, 0, 5, 0);		
					} else {
						//滴声提示已响应后台操作
						Beep(100);
						//初始化枪数据
						InitStartChgDate( &MainInfo, STARTCHARGETYPE_PLATFORM, 0, PlatformPara);
						InsertChargingRecord(&MainInfo.ChgDat);						
						//显示余额
						DispRemainMoneyInfo(PlatformPara);
						OSTimeDlyHMSM (0, 0, 2, 0);							
						//先提示启动充电中界面
						DispStartChgInfo();
						OSTimeDlyHMSM (0, 0, 0, 500);				
						//启动充电
						ret = StartCharge();
						if (ret == 0){//启动成功
							OneGunState = A_CHARGE;
						} else {
							MainInfo.ChgDat.StopCause = GetStopCause();//GetCCBStopCause(0);
							//生成充电记录,并删除充电过程数据
							ReportChargeData = 1;
							InsertChargeRecord(&MainInfo.ChgDat);
							DeleteChargingRecord(0);
							OneGunState = A_STOP;
						}
					}
					
				}
					
			}
			break;
		
		case A_CHARGE://充电---------------------------------------------------------------------------------------------------------------------------------
			info = GetChgInfo();
			//设置指示灯状态
			if ((info.ScramState == 1) || (ElmGetCommState() == 1) || (LifeTimeExpireFlag == 1) || (info.HighVol == 1)|| (info.LowVol == 1)){//充电桩故障
				SetLEDState(0, 2);
			} else {//充电桩正常
				SetLEDState(0, 1);
			}	
			UpdateChargeData(&MainInfo, info, 0);
			//更新显示屏数据
			DispAChgInfo( MainInfo.ChgDat.SumEnergy,MainInfo.ChgDat.SumMoney, MainInfo.GunInfo.CurrentA, MainInfo.GunInfo.VoltageA, MainInfo.ChgDat.SumTime );
		
			//判断并处理刷离线卡操作
			if (SwingOfflineCardFlag == 1){//刷离线卡
				SwingOfflineCardFlag = 0;
			
				if ((MainInfo.ChgDat.StartType == STARTCHARGETYPE_OFFLINECARD) && (CardID == MainInfo.ChgDat.StartCardID)){//刷卡停止
					//解锁
					money = 0;
					if (CardBalance > MainInfo.ChgDat.SumMoney)
						money = CardBalance - MainInfo.ChgDat.SumMoney;
					if(UnLockCardPro(CardID, money)){
						DeleteLockCardRecord(CardID);	
						//滴声提示已响应操作
						Beep(100);
						//停止充电
						StopCharge();
						MainInfo.ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;		
						//生成充电记录,并删除充电过程数据
						ReportChargeData = 1;
						InsertChargeRecord(&MainInfo.ChgDat );
						DeleteChargingRecord(0);
						//设置状态
						OneGunState = A_IDLE;
						//显示结算界面
						DispAccount1(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
						OSTimeDlyHMSM (0, 0, 4, 0);
						DispAccount2(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
						OSTimeDlyHMSM (0, 0, 3, 0);

					}else{			
						Beep(100);
						//停止充电
						StopCharge();
						MainInfo.ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;
						//设置状态
						OneGunState = A_STOP;					
						//生成充电记录,并删除充电过程数据
						ReportChargeData = 1;
						InsertChargeRecord(&MainInfo.ChgDat);
						DeleteChargingRecord(0);
					}
				} else if(CardLockState == 2){//是被锁定的卡
					//进入解锁卡程序
					LockedCardHandle(CardID, CardBalance);
					
				} else {
					//滴声提示已响应操作
					Beep(100);			
					//提示无空闲车位
					DispNoPileUsed();
					OSTimeDlyHMSM (0, 0, 5, 0);
				}
			}
			
			//判断并处理刷在线卡操作
			if (SwingOnlineCardFlag == 1){//刷在线卡
				SwingOnlineCardFlag = 0;
				
				if ((MainInfo.ChgDat.StartType == STARTCHARGETYPE_ONLINECARD) && (CardID == MainInfo.ChgDat.StartCardID)){//刷卡停止
					//计算余额
					money = 0;
					if (MainInfo.ChgDat.StartCardMoney > MainInfo.ChgDat.SumMoney)
						money = MainInfo.ChgDat.StartCardMoney - MainInfo.ChgDat.SumMoney;
					
					//滴声提示已响应操作
					Beep(100);
					
					//停止充电
					StopCharge();
					MainInfo.ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;
					//生成充电记录,并删除充电过程数据
					ReportChargeData= 1;
					InsertChargeRecord(&MainInfo.ChgDat );
					DeleteChargingRecord(0);					
					//设置状态
					OneGunState = A_IDLE;
					//显示结算界面
					DispAccount1(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
					OSTimeDlyHMSM (0, 0, 4, 0);
					DispAccount2(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
					OSTimeDlyHMSM (0, 0, 3, 0);

				}else {
					//滴声提示已响应操作
					Beep(100);			
					//提示无空闲车位
					DispNoPileUsed();
					OSTimeDlyHMSM (0, 0, 5, 0);
				}
			}
			
			//判断并处理后台停止操作
			if ((SetMainChargeFlag == 2) && (MainInfo.ChgDat.StartType == STARTCHARGETYPE_PLATFORM)){//后台停止
				SetMainChargeFlag = 0;			
				//计算余额
				money = MainInfo.ChgDat.StartCardMoney - MainInfo.ChgDat.SumMoney;				
				//峰鸣器响提示后台操作已被处理
				Beep(100);				
				//停止充电
				StopCharge();
				MainInfo.ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;					
				//生成充电记录,并删除充电过程数据
				ReportChargeData = 1;
				InsertChargeRecord(&MainInfo.ChgDat);
				DeleteChargingRecord(0);
				//设置为空闲状态
				OneGunState = A_IDLE;						
				//显示结算界面
				DispAccount1(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
				OSTimeDlyHMSM (0, 0, 4, 0);
				DispAccount2(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
				OSTimeDlyHMSM (0, 0, 3, 0);
			}
			if (ConditionStop(&MainInfo) == 1){//达到条件
				//峰鸣器响
				Beep(100);				
				//停止充电
				StopCharge();
				//生成充电记录,并删除充电过程数据
				ReportChargeData = 1;
				InsertChargeRecord(&MainInfo.ChgDat);
				DeleteChargingRecord(0);				
				//设置状态
				if ((MainInfo.ChgDat.StartType == STARTCHARGETYPE_OFFLINECARD) || (MainInfo.ChgDat.StartType == STARTCHARGETYPE_ONLINECARD))
					OneGunState = A_STOP;
				else
					OneGunState = A_IDLE;
			}		
			//判断A枪充电是否自动停止
			if (info.ChargeState != 1){
					//收集停止原因
				MainInfo.ChgDat.StopCause = GetStopCause();
				//生成充电记录,并删除充电过程数据
				ReportChargeData = 1;
				InsertChargeRecord(&MainInfo.ChgDat);
				DeleteChargingRecord(0);
				//设置状态
				if ((MainInfo.ChgDat.StartType == STARTCHARGETYPE_OFFLINECARD) || (MainInfo.ChgDat.StartType == STARTCHARGETYPE_ONLINECARD))
					OneGunState = A_STOP;
				else
					OneGunState = A_IDLE;
			}			
			break;
			
		case A_STOP://停止---------------------------------------------------------------------------------------------------------------------------------
			//获取充电板信息
			info = GetChgInfo();
			//设置指示灯状态
			if ((info.ScramState == 1) || (ElmGetCommState() == 1) || (LifeTimeExpireFlag == 1) || (info.HighVol == 1)|| (info.LowVol == 1)){//充电桩故障
				SetLEDState(0, 2);
			} else {//充电桩正常
				SetLEDState(0, 0);
			}
			//更新显示屏数据
			DispAStopInfo(MainInfo.ChgDat.StopCause,MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney );
				
			if (info.GunSeatState == 0) {//未插枪
				//设置状态
				OSTimeDlyHMSM (0, 0, 5, 0);
				OneGunState = A_IDLE;
			} 			
			//判断并处理刷离线卡操作
			if (SwingOfflineCardFlag == 1){//刷离线卡
				SwingOfflineCardFlag = 0;			
				if ((MainInfo.ChgDat.StartType == STARTCHARGETYPE_OFFLINECARD) && (CardID == MainInfo.ChgDat.StartCardID)){//刷卡停止
					//解锁
					money = 0;
					if (CardBalance > MainInfo.ChgDat.SumMoney)
						money = CardBalance - MainInfo.ChgDat.SumMoney;
					if(UnLockCardPro(CardID, money)){
						Beep(100);					
						//删除存储器里面的锁卡信息
						DeleteLockCardRecord(CardID);			
						//显示结算信息
						DispAccount1(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
						OSTimeDlyHMSM (0, 0, 4, 0);
						DispAccount2(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
						OSTimeDlyHMSM (0, 0, 3, 0);
						//设置状态
						OneGunState = A_IDLE;
					}else{
//						OSTimeDlyHMSM (0, 0, 2, 0);
					}
				} else if(CardLockState == 2){//是被锁定的卡
					//进入解锁卡程序
					LockedCardHandle(CardID, CardBalance);
				
				} else {
					//滴声提示已响应操作
					Beep(100);			
					//提示无空闲车位
					DispNoPileUsed();
					OSTimeDlyHMSM (0, 0, 5, 0);
				}
			}		
			//判断并处理刷在线卡操作
			if (SwingOnlineCardFlag == 1){//刷在线卡
				SwingOnlineCardFlag = 0;		
				if ((MainInfo.ChgDat.StartType == STARTCHARGETYPE_ONLINECARD) && (CardID == MainInfo.ChgDat.StartCardID)){//刷卡停止
					//计算余额
					money = 0;
					if (MainInfo.ChgDat.StartCardMoney > MainInfo.ChgDat.SumMoney)
						money = MainInfo.ChgDat.StartCardMoney - MainInfo.ChgDat.SumMoney;						
					//滴声提示已响应操作
					Beep(100);
					//设置状态
					OneGunState = A_IDLE;			
					//显示结算界面
					DispAccount1(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
					OSTimeDlyHMSM (0, 0, 4, 0);
					DispAccount2(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
					OSTimeDlyHMSM (0, 0, 3, 0);
				} else {
					//滴声提示已响应操作
					Beep(100);			
					//提示无空闲车位
					DispNoPileUsed();
					OSTimeDlyHMSM (0, 0, 5, 0);
				}
			}			
			break;
	}
}



/************************************************************************************************************
** 函 数 名 : BoardRunLedControl
** 功能描述 : 主板运行指示灯控制函数 1秒亮 1秒灭
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void BoardRunLedControl(void)
{
	static uint8_t state,poweron = 0x5a;
	static uint32_t timer;
	uint8_t i;
	
	if (poweron == 0x5a){
		poweron = 0;
		//运行指示灯上电快闪10次
		for(i=0;i<10;i++){
			LEDRun = 1;
			OSTimeDlyHMSM (0, 0, 0, 100);
			LEDRun = 0;
			OSTimeDlyHMSM (0, 0, 0, 100);
		}
	}
	if (TimerRead() - timer > T1S * 1){
		timer = TimerRead();
		if (state == 0){
			state = 1;
			LEDRun = 1;
		} else {
			state = 0;
			LEDRun = 0;
		}
	}
}



/************************************************************************************************************
** 函 数 名 : LEDBoardControl
** 功能描述 : 三色灯板控制函数
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void LEDBoardControl(void)
{
	if (LEDState == 2 ){//故障
		LEDRed = 1;
		LEDYellow = 0;
		LEDGreen =1;
	}else if (LEDState == 1){//A枪充电
		LEDRed = 0;
		LEDYellow = 1;
		LEDGreen =1;
	} else if(LEDState == 0){//A枪待机
		LEDRed = 0;
		LEDYellow = 0;
		LEDGreen =1;
	}
	 
}

/************************************************************************************************************
** 函 数 名 : SetMainCharge
** 功能描述 : 设置主充电函数
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......  setcmd SET_START_CHARGE/SET_STOP_CHARGE
							mode 模式 0充满 1按电量 2按时间 3按金额
							para 参数 按充满时精度为0.01元 按电量时精度为0.01kWh 按时间时精度为秒 按金额时精度为0.01元
							CardOrDiscernNumber 充电卡号/用户识别号 32个字节ASCII
							userid 用户ID
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void SetMainCharge(uint8_t port, uint8_t setcmd, uint8_t mode, uint32_t para, uint8_t *CardOrDiscernNumber, uint8_t *userid)
{
	if (port < 2){
		if (setcmd == SET_START_CHARGE){
			SetMainChargeFlag = 1;
			PlatformMode = mode;//平台启动时的模式 0充满 1按电量 2按时间 3按金额
			PlatformPara = para;//平台启动时的参数 按充满时精度为0.01元 按电量时精度为0.01kWh 按时间时精度为秒 按金额时精度为0.01元
			memcpy(PlatformCardOrDiscernNumber[port], CardOrDiscernNumber, 32);//平台启动时的充电卡号/用户识别号
			memcpy(PlatformUserID, userid, 2);//用户ID
		} else {
			SetMainChargeFlag = 2;
		}
	}
}

/************************************************************************************************************
** 函 数 名 : GetMainCharge
** 功能描述 : 获取主充电状态
** 输    入 : 无
** 输    出 : 无
** 返    回 :	STATE_IDLE/STATE_CHARGE/STATE_COMPLETE
*************************************************************************************************************
*/
uint8_t GetMainCharge(uint8_t port)
{
	if (OneGunState == A_IDLE){
		return STATE_IDLE;
	} else if (OneGunState == A_CHARGE){
		return STATE_CHARGE;
	} else {
		return STATE_COMPLETE;
	}
}



/************************************************************************************************************
** 函 数 名 : GetChargeRecord
** 功能描述 : 从内存中获取充电记录数据
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪  ChgData  存储的充电记录数据地址
** 输    出 : 无
** 返    回 :	0  = 获取失败   1 = 获取成功 
*************************************************************************************************************
*/
uint8_t GetChargeRecord(uint8_t port, CHARGINGDATA* ChgData)
{
	uint8_t StartDateTime[6];
	memset(StartDateTime, 0, 6);
	
	if(ReportChargeData == 0)
		return 0;
	if((memcmp(MainInfo.ChgDat.StartDateTime, StartDateTime, 6)== 0))
		return 0;
	memcpy(ChgData, &MainInfo.ChgDat, sizeof(CHARGINGDATA));
	return 1;
}



/************************************************************************************************************
** 函 数 名 : MainTask
** 功能描述 : 主任务
** 输    入 : pdata 未使用
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/


void MainTask(void *pdata)
{
	DisStartDev();	//显示加载界面
	OSTimeDlyHMSM (0, 0, 5, 0);
	if(GetApplenth(0)){
		SetApplenth( 0, 0);
		DispUpdataInfo(0, 2);
		OSTimeDlyHMSM (0, 0, 5, 0);
	}
	TimerCntRun = TimerRead();
	while(1){
		OneGunHandle();
		BoardRunLedControl();
		
		AnalyseMsgFromDebugPlatform();
		AnalyseMsgFromUdpPlatform();
		
		LEDBoardControl();  //三色灯	
		OSTimeDlyHMSM (0, 0, 0, 10);
	}
}
