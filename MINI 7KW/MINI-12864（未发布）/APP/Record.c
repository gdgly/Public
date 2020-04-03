#include "sys.h"
#include "FRAM.h"
#include "Record.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include "includes.h"
#include "Cfg.h"
#include "main.h"
 CHARGERECORDINFO ChargeRecordInfo;

 LOCKCARDINFO LockCardInfo;

 CHARGINGINFO ChargingInfo;


/************************************************************************************************************
** 函 数 名 : GetChargeRecordIndex
** 功能描述 : 获取充电记录索引号
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪...... 
** 输    出 : 无
** 返    回 :	索引号
*************************************************************************************************************
*/
uint32_t GetChargeRecordIndex(uint8_t port)
{
	return ChargeRecordInfo.index;
}

/************************************************************************************************************
** 函 数 名 : GetChargeRecordNumber
** 功能描述 : 获取充电记录数量
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪...... 
** 输    出 : 无
** 返    回 :	数量
*************************************************************************************************************
*/
uint32_t GetChargeRecordNumber(uint8_t port)
{
	uint32_t i, num = 0;
	for (i = 0; i < RECORD_TOTAL_NUM; i++){
		if (ChargeRecordInfo.recordstate[i] != 0){
			num++;
		}
	}
	return num;
}

/************************************************************************************************************
** 函 数 名 : ChargeRecordInit
** 功能描述 : 充电记录初始化
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ChargeRecordInit(void)
{
	uint8_t os_err;
	uint16_t i;
	
	memset(&ChargeRecordInfo, 0, sizeof(CHARGERECORDINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramRead(FRAMADDR_CHARGERECORDINFO, (uint8_t *)&ChargeRecordInfo, sizeof(CHARGERECORDINFO));
	OSMutexPost (FramMutex);
	if(ChargeRecordInfo.Flag !=0x5a5a){
		memset(&ChargeRecordInfo, 0, sizeof(CHARGERECORDINFO));
		ChargeRecordInfo.Flag = 0x5a5a;	
		OSMutexPend (FramMutex, 0, &os_err);
		FramWrite(FRAMADDR_CHARGERECORDINFO, (uint8_t *)&ChargeRecordInfo, sizeof(CHARGERECORDINFO));
		OSMutexPost (FramMutex);
	}else if (ChargeRecordInfo.index >= RECORD_TOTAL_NUM){
		memset(&ChargeRecordInfo, 0, sizeof(CHARGERECORDINFO));
		ChargeRecordInfo.Flag = 0x5a5a;
		OSMutexPend (FramMutex, 0, &os_err);
		FramWrite(FRAMADDR_CHARGERECORDINFO, (uint8_t *)&ChargeRecordInfo, sizeof(CHARGERECORDINFO));
		OSMutexPost (FramMutex);
	} else {
		for (i = 0; i < RECORD_TOTAL_NUM; i++){
			if (ChargeRecordInfo.recordstate[i] > 1){
				memset(&ChargeRecordInfo, 0, sizeof(CHARGERECORDINFO));
				ChargeRecordInfo.Flag = 0x5a5a;
				OSMutexPend (FramMutex, 0, &os_err);
				FramWrite(FRAMADDR_CHARGERECORDINFO, (uint8_t *)&ChargeRecordInfo, sizeof(CHARGERECORDINFO));
				OSMutexPost (FramMutex);
				break;
			}
		}
	}
}

/************************************************************************************************************
** 函 数 名 : InsertChargeRecord
** 功能描述 : 插入充电记录
** 输    入 : recordbuf 记录缓冲区 recordlen 记录长度 不超过1000字节
** 输    出 : 无
** 返    回 :	0失败 1成功
*************************************************************************************************************
*/
uint8_t InsertChargeRecord(CHARGINGDATA * chgdata)
{
	uint8_t os_err;
	chgdata->Flag =0x5a5a;
	OSMutexPend (FramMutex, 0, &os_err);	
	FramWrite(FRAMADDR_CHARGERECORDDATA+ (ChargeRecordInfo.index * CHARGERECORDDATASIZE), (uint8_t *)chgdata, sizeof(CHARGINGDATA));
	ChargeRecordInfo.recordstate[ChargeRecordInfo.index] = 1;
	if ((ChargeRecordInfo.index + 1) >= RECORD_TOTAL_NUM)
		ChargeRecordInfo.index = 0;
	else
		ChargeRecordInfo.index++;
	FramWrite(FRAMADDR_CHARGERECORDINFO, (uint8_t *)&ChargeRecordInfo, sizeof(CHARGERECORDINFO));
	OSMutexPost (FramMutex);
	return 1;
}


/************************************************************************************************************
** 函 数 名 : DeleteChargeRecord
** 功能描述 : 删除充电记录
** 输    入 : index 索引号
** 输    出 : 无
** 返    回 :	0失败 1成功
*************************************************************************************************************
*/
void DeleteChargeRecord(uint16_t index)
{
	uint8_t os_err;	
	CHARGINGDATA chargedata;
	memset(&chargedata, 0, sizeof(CHARGINGDATA));
	if (index > RECORD_TOTAL_NUM)
		return;
	OSMutexPend (FramMutex, 0, &os_err);	
	ChargeRecordInfo.recordstate[ChargeRecordInfo.index] = 0;
	FramWrite(FRAMADDR_CHARGERECORDINFO, (uint8_t *)&ChargeRecordInfo, sizeof(CHARGERECORDINFO));
	FramWrite(FRAMADDR_CHARGERECORDDATA+ (index * CHARGERECORDDATASIZE), (uint8_t *)&chargedata, sizeof(CHARGINGDATA));
	OSMutexPost (FramMutex);

	return;
}

/************************************************************************************************************
** 函 数 名 : FindChargeRecord
** 功能描述 : 查找充电记录
** 输    入 : index 索引号 chgdata 充电记录
** 输    出 : 无
** 返    回 :	0失败 1成功
*************************************************************************************************************
*/
uint8_t FindChargeRecord(uint16_t index,CHARGINGDATA * chgdata)
{
	uint8_t os_err;

	if (index > RECORD_TOTAL_NUM)
		return 0;
	if (ChargeRecordInfo.recordstate[index-1] == 0){
		return 0;
	}
	OSMutexPend (FramMutex, 0, &os_err);
	FramRead(FRAMADDR_CHARGERECORDDATA+ ((index-1) * CHARGERECORDDATASIZE), (uint8_t *)chgdata, sizeof(CHARGINGDATA));
	OSMutexPost (FramMutex);
	if(chgdata->Flag == 0x5a5a)
		return 1;
	return 0;
}

/************************************************************************************************************
** 函 数 名 : LockCardRecordInit
** 功能描述 : 锁卡记录初始化
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void LockCardRecordInit(void)
{
	uint8_t os_err;
	memset(&LockCardInfo, 0, sizeof(LOCKCARDINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramRead(FRAMADDR_LOCKCARDINFO, (uint8_t *)&LockCardInfo, sizeof(LOCKCARDINFO));
	OSMutexPost (FramMutex);
	if(LockCardInfo.Flag !=0x5a5a){
		memset(&LockCardInfo, 0, sizeof(LOCKCARDINFO));
		LockCardInfo.Flag = 0x5a5a;
		OSMutexPend (FramMutex, 0, &os_err);
		FramWrite(FRAMADDR_LOCKCARDINFO, (uint8_t *)&LockCardInfo, sizeof(LOCKCARDINFO));
		OSMutexPost (FramMutex);
	}
}

/************************************************************************************************************
** 函 数 名 : InsertLockCardRecord
** 功能描述 : 插入锁卡记录
** 输    入 : CardID 卡号
** 输    出 : 无
** 返    回 :	锁卡记录号
*************************************************************************************************************
*/
uint32_t InsertLockCardRecord(uint32_t CardID)
{
	uint8_t i;
	uint8_t os_err;
	
	for (i = 0; i < LOCKCARD_TOTAL_NUM; i++){
		if (LockCardInfo.CardID[i] == CardID){
			LockCardInfo.SerialNum[i] = 0;
			LockCardInfo.CardID[i] = 0;
		}
	}
	if ((LockCardInfo.StorePoint + 1) >= LOCKCARD_TOTAL_NUM)
		LockCardInfo.StorePoint = 0;
	else
		LockCardInfo.StorePoint++;
	LockCardInfo.SerialPoint = LockCardInfo.SerialPoint + 1;
	LockCardInfo.SerialNum[LockCardInfo.StorePoint] = LockCardInfo.SerialPoint;
	LockCardInfo.CardID[LockCardInfo.StorePoint] = CardID;
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_LOCKCARDINFO, (uint8_t *)&LockCardInfo, sizeof(LOCKCARDINFO));
	OSMutexPost (FramMutex);
	
	return LockCardInfo.SerialNum[LockCardInfo.StorePoint];
}

/************************************************************************************************************
** 函 数 名 : DeleteLockCardRecord
** 功能描述 : 删除锁卡记录
** 输    入 : CardID 卡号
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DeleteLockCardRecord(uint32_t CardID)
{
	uint8_t i;
	uint8_t os_err;
	
	for (i = 0; i < LOCKCARD_TOTAL_NUM; i++){
		if (LockCardInfo.CardID[i] == CardID){
			LockCardInfo.SerialNum[i] = 0;
			LockCardInfo.CardID[i] = 0;
		}
	}
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_LOCKCARDINFO, (uint8_t *)&LockCardInfo, sizeof(LOCKCARDINFO));
	OSMutexPost (FramMutex);
}

/************************************************************************************************************
** 函 数 名 : FindLockCardRecord
** 功能描述 : 搜索锁卡记录
** 输    入 : CardID 卡号
** 输    出 : SerialNum 序列号
** 返    回 :	0失败 1成功
*************************************************************************************************************
*/
uint8_t FindLockCardRecord(uint32_t CardID, uint32_t *SerialNum)
{
	uint8_t i;
	
	for (i = 0; i < LOCKCARD_TOTAL_NUM; i++){
		if (LockCardInfo.CardID[i] == CardID){
			*SerialNum = LockCardInfo.SerialNum[i];
			return 1;
		}
	}	
	return 0;
}

/************************************************************************************************************
** 函 数 名 : ChargingRecordInit
** 功能描述 : 充电过程中记录初始化
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void ChargingRecordInit(void)
{
	uint8_t os_err;
	uint8_t *p1,*p2;
	uint16_t sum,j,len;

	memset(&ChargingInfo, 0, sizeof(CHARGINGINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramRead(FRAMADDR_CHARGINGRECORDINFO, (uint8_t *)&ChargingInfo, sizeof(CHARGINGINFO));
	OSMutexPost (FramMutex);
	p1 = (uint8_t *)&ChargingInfo.Head;
	p2 = (uint8_t *)&ChargingInfo.Tail;
	len = p2 - p1;
	sum = 0;
	
	for (j = 0; j < len; j++)
		sum += *p1++;
	if ((ChargingInfo.Head == 0x5a5a) && (sum == ChargingInfo.Tail)){
		InsertChargeRecord(&ChargingInfo.ChgDat);
	}
	memset(&ChargingInfo, 0, sizeof(CHARGINGINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGINGRECORDINFO, (uint8_t *)&ChargingInfo, sizeof(CHARGINGINFO));
	OSMutexPost (FramMutex);
	
}

/************************************************************************************************************
** 函 数 名 : InsertChargingRecord
** 功能描述 : 插入充电过程记录
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪...... recordbuf 缓冲区 recordlen 长度
** 输    出 : 无
** 返    回 :	0失败 1成功
*************************************************************************************************************
*/
uint8_t InsertChargingRecord(CHARGINGDATA * chgdata)
{
	uint8_t os_err;
	uint8_t *p1,*p2;
	uint16_t sum,j,len;

	memset(&ChargingInfo, 0, sizeof(CHARGINGINFO));
	ChargingInfo.Head = 0x5a5a;
	memcpy(&ChargingInfo.ChgDat, chgdata, sizeof(CHARGINGDATA));
	p1 = (uint8_t *)&ChargingInfo.Head;
	p2 = (uint8_t *)&ChargingInfo.Tail;
	len = p2 - p1;
	sum = 0;
	for (j = 0; j < len; j++)
		sum += *p1++;
	ChargingInfo.Tail = sum;
	
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGINGRECORDINFO, (uint8_t *)&ChargingInfo, sizeof(CHARGINGINFO));
	OSMutexPost (FramMutex);
	return 1;
}

/************************************************************************************************************
** 函 数 名 : DeleteChargingRecord
** 功能描述 : 删除充电过程中记录
** 输    入 : port 充电接口 =0代表1号枪 =1代表2号枪......
** 输    出 : 无
** 返    回 :	0失败 1成功
*************************************************************************************************************
*/
void DeleteChargingRecord(uint8_t port)
{
	uint8_t os_err;
	memset(&ChargingInfo, 0, sizeof(CHARGINGINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGINGRECORDINFO, (uint8_t *)&ChargingInfo, sizeof(CHARGINGINFO));
	OSMutexPost (FramMutex);
}


/************************************************************************************************************
** 函 数 名 : RecordInit
** 功能描述 : 记录初始化函数
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void RecordInit(void)
{
	//充电记录初始化
	ChargeRecordInit();	
	//锁卡记录初始化
	LockCardRecordInit();
	//充电过程中记录初始化
	ChargingRecordInit();
}

/************************************************************************************************************
** 函 数 名 : DeleteAllRecord
** 功能描述 : 删除所有记录函数
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DeleteAllRecord(void)
{
	uint8_t os_err;
		
	//删除充电记录信息
	memset(&ChargeRecordInfo, 0, sizeof(CHARGERECORDINFO));
	ChargeRecordInfo.Flag = 0x5a5a;
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGERECORDINFO, (uint8_t *)&ChargeRecordInfo, sizeof(CHARGERECORDINFO));
	OSMutexPost (FramMutex);
	
	//删除锁卡记录信息
	memset(&LockCardInfo, 0, sizeof(LOCKCARDINFO));
	LockCardInfo.Flag = 0x5a5a;
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_LOCKCARDINFO, (uint8_t *)&LockCardInfo, sizeof(LOCKCARDINFO));
	OSMutexPost (FramMutex);
	
	//删除充电过程中记录信息
	memset(&ChargingInfo, 0, sizeof(CHARGINGINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGINGRECORDINFO, (uint8_t *)&ChargingInfo, sizeof(CHARGINGINFO));
	OSMutexPost (FramMutex);
}
