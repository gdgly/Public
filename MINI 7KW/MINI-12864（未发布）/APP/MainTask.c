

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

static uint8_t SetMainChargeFlag;//1���� 2ֹͣ
static uint8_t PlatformMode;//ƽ̨����ʱ��ģʽ 0���� 1������ 2��ʱ�� 3�����
static uint32_t PlatformPara;//ƽ̨����ʱ�Ĳ��� ������ʱ����Ϊ0.01Ԫ ������ʱ����Ϊ0.01kWh ��ʱ��ʱ����Ϊ�� �����ʱ����Ϊ0.01Ԫ
static uint8_t PlatformCardOrDiscernNumber[1][32]; //ƽ̨����ʱ�ĳ�翨��/�û�ʶ���
static uint8_t PlatformUserID[2];//�û�ID

static uint8_t OneGunState;//��ǹ��˫ǹ״̬

static uint8_t LifeTimeExpireFlag;//ʹ�����޵��ڱ�־ 0δ���� 1����
static	uint32_t SerialNum;       //���߿� ������ˮ��




static uint8_t UpdataInFlag; //�����б�־

static uint8_t ReportChargeData;    //�ϱ�����¼��־

static uint32_t TimerCntRun; 

static uint8_t StartingChg;   //��������б�־

/************************************************************************************************************
** �� �� �� : GetStartType
** �������� : ��ȡ������ʽ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	STARTCHARGETYPE_OFFLINECARD STARTCHARGETYPE_ONLINECARD STARTCHARGETYPE_PLATFORM
*************************************************************************************************************
*/
uint8_t GetStartType(uint8_t port)
{
	return MainInfo.ChgDat.StartType;
}

/************************************************************************************************************
** �� �� �� : GetLifeTime
** �������� : ��ȡ�Ƿ����޵���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	1 ����   0 ����
*************************************************************************************************************
*/
uint8_t GetLifeTime(void)
{
	return LifeTimeExpireFlag;
}

/************************************************************************************************************
** �� �� �� : GetStartingChg
** �������� : ��ȡ�Ƿ���������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	0 ����������  1  ������
*************************************************************************************************************
*/
uint8_t GetStartingChg(uint8_t port)
{
	return StartingChg;
}

/************************************************************************************************************
** �� �� �� : GetStartCardID
** �������� : ��ȡ������ID
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	��ID
*************************************************************************************************************
*/
uint32_t GetStartCardID(uint8_t port)
{
	return MainInfo.ChgDat.StartCardID;
}

/************************************************************************************************************
** �� �� �� : GetStartMode
** �������� : ��ȡ����ģʽ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	0���� 1������ 2��ʱ�� 3�����
*************************************************************************************************************
*/
uint8_t GetStartMode(uint8_t port)
{
	return MainInfo.ChgDat.Mode;
}

/************************************************************************************************************
** �� �� �� : GetStartPara
** �������� : ��ȡ��������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	
*************************************************************************************************************
*/
uint32_t GetStartPara(uint8_t port)
{
	return MainInfo.ChgDat.StartCardMoney; //�Զ���������Ĳ�֧��
}

/************************************************************************************************************
** �� �� �� : GetVoltage
** �������� : ��ȡ��ѹֵ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : A,B,C���ѹ
** ��    �� :	��
*************************************************************************************************************
*/
void GetVoltage(uint8_t port, uint32_t *VolA, uint32_t *VolB, uint32_t *VolC)
{
	*VolA = MainInfo.GunInfo.VoltageA;
	*VolB = 0;
	*VolC = 0;
}

/************************************************************************************************************
** �� �� �� : GetCurrent
** �������� : ��ȡ����ֵ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : A,B,C�����
** ��    �� :	��
*************************************************************************************************************
*/
void GetCurrent(uint8_t port, uint32_t *CurA, uint32_t *CurB, uint32_t *CurC)
{
	*CurA = MainInfo.GunInfo.CurrentA;
	*CurB = 0;
	*CurC = 0;
}

/************************************************************************************************************
** �� �� �� : GetStartMeterEnergy
** �������� : ��ȡ����ʱ������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	���� 0.01KWH
*************************************************************************************************************
*/
uint32_t GetStartMeterEnergy(uint8_t port)
{
	return (uint32_t) MainInfo.ChgDat.StartMeterEnergy;
}

/************************************************************************************************************
** �� �� �� : GetStopMeterEnergy
** �������� : ��ȡֹͣʱ������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	���� 0.01KWH
*************************************************************************************************************
*/
uint32_t GetStopMeterEnergy(uint8_t port)
{
	return (uint32_t)MainInfo.ChgDat.StopMeterEnergy;
}

/************************************************************************************************************
** �� �� �� : GetSumEnergy
** �������� : ��ȡ�ۼƳ�����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	���� 0.01KWH
*************************************************************************************************************
*/
uint32_t GetSumEnergy(uint8_t port)
{
	return MainInfo.ChgDat.SumEnergy;
}

/************************************************************************************************************
** �� �� �� : GetSumMoney
** �������� : ��ȡ�ۼƳ����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	��� 0.01Ԫ
*************************************************************************************************************
*/
uint32_t GetSumMoney(uint8_t port)
{
	return MainInfo.ChgDat.SumMoney;
}

/************************************************************************************************************
** �� �� �� : GetPeriodEnergy
** �������� : ��ȡʱ�ε���
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	���� 0.01KWH
*************************************************************************************************************
*/
uint16_t GetPeriodEnergy(uint8_t port, uint8_t Period)
{
	return MainInfo.ChgDat.PeriodEnergy[Period];
}

/************************************************************************************************************
** �� �� �� : GetSumTime
** �������� : ��ȡ�ۼ�ʱ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� : ʱ�� 
*************************************************************************************************************
*/
uint32_t GetSumTime(uint8_t port)
{
	return MainInfo.ChgDat.SumTime;
}


/************************************************************************************************************
** �� �� �� : JudgeLifeTime
** �������� : �ж�ʹ������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : GetStartDateTime
** �������� : ��ȡ����ʱ����ʱ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ����ʱ��
** ��    �� : ��
*************************************************************************************************************
*/
void GetStartDateTime(uint8_t port, uint8_t *datetime)
{
	memcpy(datetime, MainInfo.ChgDat.StartDateTime, 6);
}

/************************************************************************************************************
** �� �� �� : GetStopDateTime
** �������� : ��ȡֹͣʱ����ʱ��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ����ʱ��
** ��    �� : ��
*************************************************************************************************************
*/
void GetStopDateTime(uint8_t port, uint8_t *datetime)
{
	memcpy(datetime, MainInfo.ChgDat.StopDateTime, 6);
}

/************************************************************************************************************
** �� �� �� : GetStartCardMoney
** �������� : ��ȡ����ʱ�����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� : ��� 0.01Ԫ
*************************************************************************************************************
*/
uint32_t GetStartCardMoney(uint8_t port)
{
	return MainInfo.ChgDat.StartCardMoney;
}



/************************************************************************************************************
** �� �� �� : SetLEDState
** �������� : ���õư���ʾ״̬
** ��    �� : port state 0���� 1��� 2����
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void SetLEDState(uint8_t port, uint8_t state)
{
	LEDState = state;
}

/************************************************************************************************************
** �� �� �� : Beep
** �������� : �����������
** ��    �� : ms �����ʱ�� ����
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void Beep(uint32_t ms)
{

}

/************************************************************************************************************
** �� �� �� : JudgePeriod
** �������� : �ж�ʱ�亯��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......    time 0-47
** ��    �� : ��
** ��    �� :	��
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
			if ((caltime >= starttime) && (caltime < stoptime)){//ǰ�պ�
				money = ParaGetFeilvMoney(port, i);
				break; 
			}
		}
	}
	return money;
}

/************************************************************************************************************
** �� �� �� : GetOnlineCardInfo
** �������� : ��ȡ���߿���Ϣ
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... CardID ���� CardBalance ���
** ��    �� : ��
** ��    �� :	��ȡ�ɹ�1 ��ȡʧ��0
*************************************************************************************************************
*/
static uint8_t GetOnlineCardInfo(uint8_t port, uint32_t CardID, uint32_t *CardBalance)
{
	uint8_t verifyonlineret;
	uint8_t TmpBuf[48];

	//��ʾ������֤�У����Ե�
	DispInVerify();
	//������ʾ����Ӧ����
	Beep(100);
	//��ʵ����Ϣ
	if ((verifyonlineret = VerifyOnlineCardInfo(port, CardID, CardBalance)) == 0){
		return 1;
	} else {
		//������ʾ����Ӧ����
		Beep(100);
		//��ʾ��֤ʧ��
		if (verifyonlineret == 0xff)
			sprintf((char *)TmpBuf, "��Ӧ��ʱ");
		else if (verifyonlineret == 2)
			sprintf((char *)TmpBuf, "����");
		else if (verifyonlineret == 5)
			sprintf((char *)TmpBuf, "���Ų�����");
		else if (verifyonlineret == 6)
			sprintf((char *)TmpBuf, "�ѹ�ʧ");
		else if (verifyonlineret == 7)
			sprintf((char *)TmpBuf, "������");
		else if (verifyonlineret == 9)
			sprintf((char *)TmpBuf, "��������׮���");
		else
			sprintf((char *)TmpBuf, "����ԭ��");
		DispVerifyFailure(verifyonlineret);
		OSTimeDlyHMSM (0, 0, 5, 0);
		return 0;
	}
}





/************************************************************************************************************
** �� �� �� : InitStartChgDate
** �������� : ��ʼ����ʼ�������
** ��    �� : Maininfo  �����Ϣ�ṹ��   
**  				:	Mode  0���߿�  3���߿�   1��̨   port ==0 Aǹ   ==1Bǹ
**  				:	CardId   ��ID  Money == StartCardMoney 
** ��    �� : ��
** ��    �� :	��
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
	printf("StartTimer��%d\r\n", Info->GunInfo.StartTimer);
	if(Mode == 0){   //  ���߿�
		Info->ChgDat.SerialNum = SerialNum;		
	}else if(Mode == 3){     //  ���߿�			
		Info->ChgDat.SerialNum = 0;	
	}else if(Mode == 1){   // ƽ̨
		Info->ChgDat.Mode = PlatformMode;
		Info->ChgDat.StartCardMoney = Money;
//		memcpy(Info->ChgDat.CardOrDiscernNumber, PlatformCardOrDiscernNumber[port], 32);
		Info->ChgDat.StartCardID = PlatformUserID[0] | PlatformUserID[1] << 8;
		Info->ChgDat.SerialNum = 0;
	}
	StartingChg = TRUE;
}


/************************************************************************************************************
** �� �� �� : UpdateChargeData
** �������� : ���³��ʱ���������
** ��    �� : ChgData  MAININFO�ṹ��ָ��   
**  				:	info  CHARGEINFO�ṹ��
**  				:	port ==0 Aǹ   ==1Bǹ
** ��    �� : ��
** ��    �� :	��
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
			if(ChgData->ChgDat.Mode == 3){ //�����
				ChgData->ChgDat.Para = UpdateMoney;
			}
			ChgData->ChgDat.StartCardMoney = UpdateMoney;
			InsertChargingRecord( &ChgData->ChgDat );
		}
	}
	//������������
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
	ChgData->ChgDat.SumMoney = (data32 + 90000)/ 100000;//ʼ�ռ�0.009Ԫ������̨�ķ���Ѻͳ��ѷֿ�������
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
//** �� �� �� : ShowMenu
//** �������� : ��ʾ �˵��������Ϣ
//** ��    �� : ��
//** ��    �� : ��
//** ��    �� :	��
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
//			sprintf((char *)linebuf, "����");
//		else
//			sprintf((char *)linebuf, "   ");
//	}else{
//		if((GetOnlineState(0) == 1) && (GetOnlineState(1) == 1)){
//			sprintf((char *)linebuf, "����");
//		} else if( GetOnlineState(0) == 1){
//			sprintf((char *)linebuf, "A����");
//		}	else if( GetOnlineState(1) == 1){
//			sprintf((char *)linebuf, "B����");
//		}else {
//			sprintf((char *)linebuf, "   ");
//		}		
//	}
//	DispMenuInfo(timebuf, linebuf );		
//}


/************************************************************************************************************
** �� �� �� : UnLockCardPro
** �������� : ���������������
** ��    �� : Cardid   ��ID   Money  д�Ľ��
** ��    �� : ��
** ��    �� :	1 ����д���ɹ�  0 ʧ��
*************************************************************************************************************
*/
static uint8_t UnLockCardPro(uint32_t cardid, uint32_t Money)
{
	uint32_t Timer;
	
	if (CardSetMoneyAndUnlock(cardid, Money)){//���������۷ѳɹ�
		return 1;
	}
	//��ʾ�����н���		
	DispOperationInfo();
	Timer = TimerRead();
	while((TimerRead() - Timer) < T1S * 10){
		OSTimeDlyHMSM (0, 0, 0, 20);
		if (CardSetMoneyAndUnlock(cardid, Money)){//���������۷ѳɹ�
			while((TimerRead() - Timer) < T1S * 2){
				OSTimeDlyHMSM (0, 0, 0, 20);
			}
			return 1;
		}		
	}
	return 0;
}




/************************************************************************************************************
** �� �� �� : LockedCardHandle
** �������� : ����������ز���
** ��    �� : cardid   ��ID   money  д�Ľ��
** ��    �� : ��
** ��    �� :	��
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
	if ((memcmp(TmpBuf, PileNumber, 4) == 0) || (FindLockCardRecord(cardid, &SerialNum) == 1)){//�ÿ�����׮��ס
		ret = 0;
		if (FindLockCardRecord(cardid, &SerialNum) == 1){
			index = GetChargeRecordIndex(0);
			RecordNumber = GetChargeRecordNumber(0);
			for (i = 0; i < RecordNumber; i++){//�����100����¼�������
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
		if (ret == 1){//��¼��ѯ�ɹ�
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
		} else {//�Ҳ���������¼
			Beep(100);
			DispUnLockFailureInfo(UNLOCKCARDFAILCODE_NORECORD);
			UnLockCardPro(cardid, 10000);
			OSTimeDlyHMSM (0, 0, 5, 0);
		}
	} else {//��ʾ����׮��ס
		Beep(100);	
		DispCardLockInfo(TmpBuf);
		UnLockCardPro(cardid, 10000);
		OSTimeDlyHMSM (0, 0, 5, 0);
	}
}




/************************************************************************************************************
** �� �� �� : LockCardPro
** �������� : ���������������
** ��    �� : Cardid   ��ID
** ��    �� : ��
** ��    �� :	1 �����ɹ�  0 ʧ��
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
		SerialNum = InsertLockCardRecord(Cardid );			//�洢������¼		
		return 1;
	}
	//��ʾ�����н���		
	DispOperationInfo();
	while((TimerRead() - Timer) < T1S * 10){
		OSTimeDlyHMSM (0, 0, 0, 20);
		if( CardSetPileNumberAndLock(Cardid, PileNumber) == 1){
			SerialNum = InsertLockCardRecord(Cardid );			//�洢������¼	
			while((TimerRead() - Timer) < T1S * 2){
				OSTimeDlyHMSM (0, 0, 0, 20);
			}
			return 1;
		}
	}
	return 0;
}




/************************************************************************************************************
** �� �� �� : ConditionStop
** �������� : �жϳ���Ƿ��Զ�ֹͣ
** ��    �� : MainInf  �����ؽṹ��   port  0 ==Aǹ    1 ==Bǹ 
** ��    �� : ��
** ��    �� :	1 ֹͣ  0 ����
*************************************************************************************************************
*/
static uint8_t ConditionStop( MAININFO* MainInf )
{
	if ((MainInf->ChgDat.SumMoney + MINIMUM_CHARGE_MONEY) > MainInf->ChgDat.StartCardMoney){//�����
		MainInf->ChgDat.StopCause = CAUSE_MONEY_FAULT;
		MainInf->ChgDat.SumMoney = MainInf->ChgDat.StartCardMoney;
		return 1;	
//	}else if ((MainInf->ChgDat.Mode == 1) && (MainInf->ChgDat.SumEnergy >= MainInf->ChgDat.Para)){//������
//		MainInf->ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;
//		return 1;
//	}else if ((MainInf->ChgDat.Mode == 2) && (MainInf->ChgDat.SumTime >= MainInf->ChgDat.Para)){//��ʱ��
//		MainInf->ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;
//		return 1;
//	}else if ((MainInf->ChgDat.Mode == 3) && (MainInf->ChgDat.SumMoney >= MainInf->ChgDat.Para)){//�����
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
** �� �� �� : OneGunHandle
** �������� : ��ǹ������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
static void OneGunHandle(void)
{
	CHARGEINFO info;
	uint8_t TmpBuf[257],*pTmpBuf,PileNumber[32];
	static uint8_t cardinflag;
	uint8_t SwingOfflineCardFlag = 0;//ˢ���߿���־ 0δˢ�� 1ˢ��
	uint8_t SwingOnlineCardFlag = 0;//ˢ���߿���־ 0δˢ�� 1ˢ��
	uint8_t CardLockState;//����״̬ 1���� 2����
	uint32_t CardID;//����
	uint32_t CardBalance;//����� 2λС��
	uint8_t ret;
	uint32_t money;
	uint16_t Updata;
	StartingChg = FALSE;
	//��ȡ����Ϣ
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


	//����ʹ������
	JudgeLifeTime();
	DisHeadInfo(GetOnlineState(0));
	switch(OneGunState){
		case A_IDLE://����---------------------------------------------------------------------------------------------------------------------------------
			info = GetChgInfo();
			Updata = GetUpdataPre();
			if(( Updata>> 8) == 1){   //������
				if(UpdataInFlag == 0)
					Beep(100);
				UpdataInFlag = 1;
				DispUpdataInfo(Updata&0xFF, 1);
			}else if(UpdataInFlag){
				 //��ʾ����ʧ��
				Beep(100);
				UpdataInFlag = 0;
				DispUpdataInfo(Updata&0xFF,3);
				OSTimeDlyHMSM (0, 0, 5, 0);	
			}else if ((info.ScramState == 1) || (ElmGetCommState() == 1) || (LifeTimeExpireFlag == 1) || (info.HighVol == 1)|| (info.LowVol == 1)){//���׮����
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
				if (SwingOfflineCardFlag == 1){   //A����B����ˢ���߿�
					SwingOfflineCardFlag = 0;
					if (CardLockState == 1){//����	
						//������ʾ����Ӧ����
						Beep(100);
						DispStartFailureInfo();
						OSTimeDlyHMSM (0, 0, 5, 0);	
					}
					if (CardLockState == 2){//������
						LockedCardHandle(CardID, CardBalance);
					}
				}
				if(SwingOnlineCardFlag == 1){
					SwingOnlineCardFlag = 0;
					//������ʾ����Ӧ����
					Beep(100);
					DispStartFailureInfo();
					OSTimeDlyHMSM (0, 0, 5, 0);	
				}			
			} else {//���׮����
				//����ָʾ����ʾ״̬
				SetLEDState(0, 0);
				
				//������ʾ������
				ParaGetPileNo(0, PileNumber);
				strcat((char* )PileNumber,"A");
				DispAIdleInfo(info.GunSeatState, PileNumber);	
					//�жϲ�����ˢ���߿�����
				if (SwingOfflineCardFlag == 1){//ˢ���߿�
					SwingOfflineCardFlag = 0;		
					if (CardLockState == 1){//������
						if (info.GunSeatState == 0){//δ��ǹ
							//������ʾ����Ӧ����
							Beep(100);						
							//��ʾδ��ǹ
							DispInsertGunInfo();
							OSTimeDlyHMSM (0, 0, 5, 0);
							
						} else {						
							if( LockCardPro(CardID)){
								//������ʾ����Ӧ����
								Beep(100);
								//��ʼ��ǹ����								
								InitStartChgDate( &MainInfo, STARTCHARGETYPE_OFFLINECARD, CardID, CardBalance);
								InsertChargingRecord( &MainInfo.ChgDat);	
						
								//��ʾ�����
								DispRemainMoneyInfo(CardBalance);
								OSTimeDlyHMSM (0, 0, 2, 0);	
								
								//��ʾ�����н���
								DispStartChgInfo();
								OSTimeDlyHMSM (0, 0, 0, 500);
								
								//��������
								ret = StartCharge();
								if (ret == 0){//�����ɹ�
									OneGunState = A_CHARGE;
								} else {
									MainInfo.ChgDat.StopCause = GetStopCause();//GetCCBStopCause(0);
									//���ɳ���¼,��ɾ������������
									ReportChargeData = 1;
									InsertChargeRecord( &MainInfo.ChgDat);
									DeleteChargingRecord(0);
									OneGunState = A_STOP;
									DispStartFailureInfo(); //��ʾ����ʧ��
									OSTimeDlyHMSM (0, 0, 2, 0);
								}
							}else{
								DispStartFailureInfo();
								OSTimeDlyHMSM (0, 0, 5, 0);
							}		
						}
					}
					if (CardLockState == 2){//������
						LockedCardHandle(CardID, CardBalance);
					}
				}	else if (SwingOnlineCardFlag == 1){//ˢ���߿�
					SwingOnlineCardFlag = 0;	
					if (info.GunSeatState == 0){//δ��ǹ
						//������ʾ����Ӧ����
						Beep(100);
						//��ʾδ��ǹ
						DispInsertGunInfo();
						OSTimeDlyHMSM (0, 0, 5, 0);
						
					} else {
						//��ȡ���߿���Ϣ
						if (GetOnlineCardInfo(0, CardID, &CardBalance) == 1){
							//��ʼ��ǹ����
							InitStartChgDate( &MainInfo, STARTCHARGETYPE_ONLINECARD, CardID, CardBalance);
							InsertChargingRecord(&MainInfo.ChgDat);						
							//��ʾ�����
							DispRemainMoneyInfo(CardBalance );
							OSTimeDlyHMSM (0, 0, 2, 0);							
							//����ʾ��������н���
							DispStartChgInfo();
							OSTimeDlyHMSM (0, 0, 0, 500);						
							//�������
							ret = StartCharge();
							if (ret == 0){//�����ɹ�
								OneGunState = A_CHARGE;
							} else {
								MainInfo.ChgDat.StopCause = GetStopCause();//GetCCBStopCause(0);
								//���ɳ���¼,��ɾ������������
								ReportChargeData = 1;
								InsertChargeRecord( &MainInfo.ChgDat);
								DeleteChargingRecord(0);
								OneGunState = A_STOP;
							}
						}else {
						
						}
					}
				} else if (SetMainChargeFlag == 1) {//��̨����     				//�жϲ������̨����
					SetMainChargeFlag = 0;				
					if (info.GunSeatState == 0){//δ��ǹ
						//������ʾ����Ӧ����
						Beep(100);
						//��ʾδ��ǹ
						DispInsertGunInfo();
						OSTimeDlyHMSM (0, 0, 5, 0);		
					} else {
						//������ʾ����Ӧ��̨����
						Beep(100);
						//��ʼ��ǹ����
						InitStartChgDate( &MainInfo, STARTCHARGETYPE_PLATFORM, 0, PlatformPara);
						InsertChargingRecord(&MainInfo.ChgDat);						
						//��ʾ���
						DispRemainMoneyInfo(PlatformPara);
						OSTimeDlyHMSM (0, 0, 2, 0);							
						//����ʾ��������н���
						DispStartChgInfo();
						OSTimeDlyHMSM (0, 0, 0, 500);				
						//�������
						ret = StartCharge();
						if (ret == 0){//�����ɹ�
							OneGunState = A_CHARGE;
						} else {
							MainInfo.ChgDat.StopCause = GetStopCause();//GetCCBStopCause(0);
							//���ɳ���¼,��ɾ������������
							ReportChargeData = 1;
							InsertChargeRecord(&MainInfo.ChgDat);
							DeleteChargingRecord(0);
							OneGunState = A_STOP;
						}
					}
					
				}
					
			}
			break;
		
		case A_CHARGE://���---------------------------------------------------------------------------------------------------------------------------------
			info = GetChgInfo();
			//����ָʾ��״̬
			if ((info.ScramState == 1) || (ElmGetCommState() == 1) || (LifeTimeExpireFlag == 1) || (info.HighVol == 1)|| (info.LowVol == 1)){//���׮����
				SetLEDState(0, 2);
			} else {//���׮����
				SetLEDState(0, 1);
			}	
			UpdateChargeData(&MainInfo, info, 0);
			//������ʾ������
			DispAChgInfo( MainInfo.ChgDat.SumEnergy,MainInfo.ChgDat.SumMoney, MainInfo.GunInfo.CurrentA, MainInfo.GunInfo.VoltageA, MainInfo.ChgDat.SumTime );
		
			//�жϲ�����ˢ���߿�����
			if (SwingOfflineCardFlag == 1){//ˢ���߿�
				SwingOfflineCardFlag = 0;
			
				if ((MainInfo.ChgDat.StartType == STARTCHARGETYPE_OFFLINECARD) && (CardID == MainInfo.ChgDat.StartCardID)){//ˢ��ֹͣ
					//����
					money = 0;
					if (CardBalance > MainInfo.ChgDat.SumMoney)
						money = CardBalance - MainInfo.ChgDat.SumMoney;
					if(UnLockCardPro(CardID, money)){
						DeleteLockCardRecord(CardID);	
						//������ʾ����Ӧ����
						Beep(100);
						//ֹͣ���
						StopCharge();
						MainInfo.ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;		
						//���ɳ���¼,��ɾ������������
						ReportChargeData = 1;
						InsertChargeRecord(&MainInfo.ChgDat );
						DeleteChargingRecord(0);
						//����״̬
						OneGunState = A_IDLE;
						//��ʾ�������
						DispAccount1(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
						OSTimeDlyHMSM (0, 0, 4, 0);
						DispAccount2(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
						OSTimeDlyHMSM (0, 0, 3, 0);

					}else{			
						Beep(100);
						//ֹͣ���
						StopCharge();
						MainInfo.ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;
						//����״̬
						OneGunState = A_STOP;					
						//���ɳ���¼,��ɾ������������
						ReportChargeData = 1;
						InsertChargeRecord(&MainInfo.ChgDat);
						DeleteChargingRecord(0);
					}
				} else if(CardLockState == 2){//�Ǳ������Ŀ�
					//�������������
					LockedCardHandle(CardID, CardBalance);
					
				} else {
					//������ʾ����Ӧ����
					Beep(100);			
					//��ʾ�޿��г�λ
					DispNoPileUsed();
					OSTimeDlyHMSM (0, 0, 5, 0);
				}
			}
			
			//�жϲ�����ˢ���߿�����
			if (SwingOnlineCardFlag == 1){//ˢ���߿�
				SwingOnlineCardFlag = 0;
				
				if ((MainInfo.ChgDat.StartType == STARTCHARGETYPE_ONLINECARD) && (CardID == MainInfo.ChgDat.StartCardID)){//ˢ��ֹͣ
					//�������
					money = 0;
					if (MainInfo.ChgDat.StartCardMoney > MainInfo.ChgDat.SumMoney)
						money = MainInfo.ChgDat.StartCardMoney - MainInfo.ChgDat.SumMoney;
					
					//������ʾ����Ӧ����
					Beep(100);
					
					//ֹͣ���
					StopCharge();
					MainInfo.ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;
					//���ɳ���¼,��ɾ������������
					ReportChargeData= 1;
					InsertChargeRecord(&MainInfo.ChgDat );
					DeleteChargingRecord(0);					
					//����״̬
					OneGunState = A_IDLE;
					//��ʾ�������
					DispAccount1(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
					OSTimeDlyHMSM (0, 0, 4, 0);
					DispAccount2(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
					OSTimeDlyHMSM (0, 0, 3, 0);

				}else {
					//������ʾ����Ӧ����
					Beep(100);			
					//��ʾ�޿��г�λ
					DispNoPileUsed();
					OSTimeDlyHMSM (0, 0, 5, 0);
				}
			}
			
			//�жϲ������ֹ̨ͣ����
			if ((SetMainChargeFlag == 2) && (MainInfo.ChgDat.StartType == STARTCHARGETYPE_PLATFORM)){//��ֹ̨ͣ
				SetMainChargeFlag = 0;			
				//�������
				money = MainInfo.ChgDat.StartCardMoney - MainInfo.ChgDat.SumMoney;				
				//����������ʾ��̨�����ѱ�����
				Beep(100);				
				//ֹͣ���
				StopCharge();
				MainInfo.ChgDat.StopCause = CAUSE_USER_NORMAL_STOP;					
				//���ɳ���¼,��ɾ������������
				ReportChargeData = 1;
				InsertChargeRecord(&MainInfo.ChgDat);
				DeleteChargingRecord(0);
				//����Ϊ����״̬
				OneGunState = A_IDLE;						
				//��ʾ�������
				DispAccount1(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
				OSTimeDlyHMSM (0, 0, 4, 0);
				DispAccount2(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
				OSTimeDlyHMSM (0, 0, 3, 0);
			}
			if (ConditionStop(&MainInfo) == 1){//�ﵽ����
				//��������
				Beep(100);				
				//ֹͣ���
				StopCharge();
				//���ɳ���¼,��ɾ������������
				ReportChargeData = 1;
				InsertChargeRecord(&MainInfo.ChgDat);
				DeleteChargingRecord(0);				
				//����״̬
				if ((MainInfo.ChgDat.StartType == STARTCHARGETYPE_OFFLINECARD) || (MainInfo.ChgDat.StartType == STARTCHARGETYPE_ONLINECARD))
					OneGunState = A_STOP;
				else
					OneGunState = A_IDLE;
			}		
			//�ж�Aǹ����Ƿ��Զ�ֹͣ
			if (info.ChargeState != 1){
					//�ռ�ֹͣԭ��
				MainInfo.ChgDat.StopCause = GetStopCause();
				//���ɳ���¼,��ɾ������������
				ReportChargeData = 1;
				InsertChargeRecord(&MainInfo.ChgDat);
				DeleteChargingRecord(0);
				//����״̬
				if ((MainInfo.ChgDat.StartType == STARTCHARGETYPE_OFFLINECARD) || (MainInfo.ChgDat.StartType == STARTCHARGETYPE_ONLINECARD))
					OneGunState = A_STOP;
				else
					OneGunState = A_IDLE;
			}			
			break;
			
		case A_STOP://ֹͣ---------------------------------------------------------------------------------------------------------------------------------
			//��ȡ������Ϣ
			info = GetChgInfo();
			//����ָʾ��״̬
			if ((info.ScramState == 1) || (ElmGetCommState() == 1) || (LifeTimeExpireFlag == 1) || (info.HighVol == 1)|| (info.LowVol == 1)){//���׮����
				SetLEDState(0, 2);
			} else {//���׮����
				SetLEDState(0, 0);
			}
			//������ʾ������
			DispAStopInfo(MainInfo.ChgDat.StopCause,MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney );
				
			if (info.GunSeatState == 0) {//δ��ǹ
				//����״̬
				OSTimeDlyHMSM (0, 0, 5, 0);
				OneGunState = A_IDLE;
			} 			
			//�жϲ�����ˢ���߿�����
			if (SwingOfflineCardFlag == 1){//ˢ���߿�
				SwingOfflineCardFlag = 0;			
				if ((MainInfo.ChgDat.StartType == STARTCHARGETYPE_OFFLINECARD) && (CardID == MainInfo.ChgDat.StartCardID)){//ˢ��ֹͣ
					//����
					money = 0;
					if (CardBalance > MainInfo.ChgDat.SumMoney)
						money = CardBalance - MainInfo.ChgDat.SumMoney;
					if(UnLockCardPro(CardID, money)){
						Beep(100);					
						//ɾ���洢�������������Ϣ
						DeleteLockCardRecord(CardID);			
						//��ʾ������Ϣ
						DispAccount1(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
						OSTimeDlyHMSM (0, 0, 4, 0);
						DispAccount2(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
						OSTimeDlyHMSM (0, 0, 3, 0);
						//����״̬
						OneGunState = A_IDLE;
					}else{
//						OSTimeDlyHMSM (0, 0, 2, 0);
					}
				} else if(CardLockState == 2){//�Ǳ������Ŀ�
					//�������������
					LockedCardHandle(CardID, CardBalance);
				
				} else {
					//������ʾ����Ӧ����
					Beep(100);			
					//��ʾ�޿��г�λ
					DispNoPileUsed();
					OSTimeDlyHMSM (0, 0, 5, 0);
				}
			}		
			//�жϲ�����ˢ���߿�����
			if (SwingOnlineCardFlag == 1){//ˢ���߿�
				SwingOnlineCardFlag = 0;		
				if ((MainInfo.ChgDat.StartType == STARTCHARGETYPE_ONLINECARD) && (CardID == MainInfo.ChgDat.StartCardID)){//ˢ��ֹͣ
					//�������
					money = 0;
					if (MainInfo.ChgDat.StartCardMoney > MainInfo.ChgDat.SumMoney)
						money = MainInfo.ChgDat.StartCardMoney - MainInfo.ChgDat.SumMoney;						
					//������ʾ����Ӧ����
					Beep(100);
					//����״̬
					OneGunState = A_IDLE;			
					//��ʾ�������
					DispAccount1(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
					OSTimeDlyHMSM (0, 0, 4, 0);
					DispAccount2(MainInfo.ChgDat.SumEnergy, MainInfo.ChgDat.SumMoney, money, MainInfo.ChgDat.SumTime, MainInfo.ChgDat.StopDateTime);
					OSTimeDlyHMSM (0, 0, 3, 0);
				} else {
					//������ʾ����Ӧ����
					Beep(100);			
					//��ʾ�޿��г�λ
					DispNoPileUsed();
					OSTimeDlyHMSM (0, 0, 5, 0);
				}
			}			
			break;
	}
}



/************************************************************************************************************
** �� �� �� : BoardRunLedControl
** �������� : ��������ָʾ�ƿ��ƺ��� 1���� 1����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void BoardRunLedControl(void)
{
	static uint8_t state,poweron = 0x5a;
	static uint32_t timer;
	uint8_t i;
	
	if (poweron == 0x5a){
		poweron = 0;
		//����ָʾ���ϵ����10��
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
** �� �� �� : LEDBoardControl
** �������� : ��ɫ�ư���ƺ���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void LEDBoardControl(void)
{
	if (LEDState == 2 ){//����
		LEDRed = 1;
		LEDYellow = 0;
		LEDGreen =1;
	}else if (LEDState == 1){//Aǹ���
		LEDRed = 0;
		LEDYellow = 1;
		LEDGreen =1;
	} else if(LEDState == 0){//Aǹ����
		LEDRed = 0;
		LEDYellow = 0;
		LEDGreen =1;
	}
	 
}

/************************************************************************************************************
** �� �� �� : SetMainCharge
** �������� : ��������纯��
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......  setcmd SET_START_CHARGE/SET_STOP_CHARGE
							mode ģʽ 0���� 1������ 2��ʱ�� 3�����
							para ���� ������ʱ����Ϊ0.01Ԫ ������ʱ����Ϊ0.01kWh ��ʱ��ʱ����Ϊ�� �����ʱ����Ϊ0.01Ԫ
							CardOrDiscernNumber ��翨��/�û�ʶ��� 32���ֽ�ASCII
							userid �û�ID
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void SetMainCharge(uint8_t port, uint8_t setcmd, uint8_t mode, uint32_t para, uint8_t *CardOrDiscernNumber, uint8_t *userid)
{
	if (port < 2){
		if (setcmd == SET_START_CHARGE){
			SetMainChargeFlag = 1;
			PlatformMode = mode;//ƽ̨����ʱ��ģʽ 0���� 1������ 2��ʱ�� 3�����
			PlatformPara = para;//ƽ̨����ʱ�Ĳ��� ������ʱ����Ϊ0.01Ԫ ������ʱ����Ϊ0.01kWh ��ʱ��ʱ����Ϊ�� �����ʱ����Ϊ0.01Ԫ
			memcpy(PlatformCardOrDiscernNumber[port], CardOrDiscernNumber, 32);//ƽ̨����ʱ�ĳ�翨��/�û�ʶ���
			memcpy(PlatformUserID, userid, 2);//�û�ID
		} else {
			SetMainChargeFlag = 2;
		}
	}
}

/************************************************************************************************************
** �� �� �� : GetMainCharge
** �������� : ��ȡ�����״̬
** ��    �� : ��
** ��    �� : ��
** ��    �� :	STATE_IDLE/STATE_CHARGE/STATE_COMPLETE
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
** �� �� �� : GetChargeRecord
** �������� : ���ڴ��л�ȡ����¼����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ  ChgData  �洢�ĳ���¼���ݵ�ַ
** ��    �� : ��
** ��    �� :	0  = ��ȡʧ��   1 = ��ȡ�ɹ� 
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
** �� �� �� : MainTask
** �������� : ������
** ��    �� : pdata δʹ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/


void MainTask(void *pdata)
{
	DisStartDev();	//��ʾ���ؽ���
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
		
		LEDBoardControl();  //��ɫ��	
		OSTimeDlyHMSM (0, 0, 0, 10);
	}
}
