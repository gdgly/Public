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
** �� �� �� : GetChargeRecordIndex
** �������� : ��ȡ����¼������
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	������
*************************************************************************************************************
*/
uint32_t GetChargeRecordIndex(uint8_t port)
{
	return ChargeRecordInfo.index;
}

/************************************************************************************************************
** �� �� �� : GetChargeRecordNumber
** �������� : ��ȡ����¼����
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... 
** ��    �� : ��
** ��    �� :	����
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
** �� �� �� : ChargeRecordInit
** �������� : ����¼��ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : InsertChargeRecord
** �������� : �������¼
** ��    �� : recordbuf ��¼������ recordlen ��¼���� ������1000�ֽ�
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
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
** �� �� �� : DeleteChargeRecord
** �������� : ɾ������¼
** ��    �� : index ������
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
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
** �� �� �� : FindChargeRecord
** �������� : ���ҳ���¼
** ��    �� : index ������ chgdata ����¼
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
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
** �� �� �� : LockCardRecordInit
** �������� : ������¼��ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : InsertLockCardRecord
** �������� : ����������¼
** ��    �� : CardID ����
** ��    �� : ��
** ��    �� :	������¼��
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
** �� �� �� : DeleteLockCardRecord
** �������� : ɾ��������¼
** ��    �� : CardID ����
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : FindLockCardRecord
** �������� : ����������¼
** ��    �� : CardID ����
** ��    �� : SerialNum ���к�
** ��    �� :	0ʧ�� 1�ɹ�
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
** �� �� �� : ChargingRecordInit
** �������� : �������м�¼��ʼ��
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
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
** �� �� �� : InsertChargingRecord
** �������� : ��������̼�¼
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ...... recordbuf ������ recordlen ����
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
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
** �� �� �� : DeleteChargingRecord
** �������� : ɾ���������м�¼
** ��    �� : port ���ӿ� =0����1��ǹ =1����2��ǹ......
** ��    �� : ��
** ��    �� :	0ʧ�� 1�ɹ�
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
** �� �� �� : RecordInit
** �������� : ��¼��ʼ������
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void RecordInit(void)
{
	//����¼��ʼ��
	ChargeRecordInit();	
	//������¼��ʼ��
	LockCardRecordInit();
	//�������м�¼��ʼ��
	ChargingRecordInit();
}

/************************************************************************************************************
** �� �� �� : DeleteAllRecord
** �������� : ɾ�����м�¼����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DeleteAllRecord(void)
{
	uint8_t os_err;
		
	//ɾ������¼��Ϣ
	memset(&ChargeRecordInfo, 0, sizeof(CHARGERECORDINFO));
	ChargeRecordInfo.Flag = 0x5a5a;
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGERECORDINFO, (uint8_t *)&ChargeRecordInfo, sizeof(CHARGERECORDINFO));
	OSMutexPost (FramMutex);
	
	//ɾ��������¼��Ϣ
	memset(&LockCardInfo, 0, sizeof(LOCKCARDINFO));
	LockCardInfo.Flag = 0x5a5a;
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_LOCKCARDINFO, (uint8_t *)&LockCardInfo, sizeof(LOCKCARDINFO));
	OSMutexPost (FramMutex);
	
	//ɾ���������м�¼��Ϣ
	memset(&ChargingInfo, 0, sizeof(CHARGINGINFO));
	OSMutexPend (FramMutex, 0, &os_err);
	FramWrite(FRAMADDR_CHARGINGRECORDINFO, (uint8_t *)&ChargingInfo, sizeof(CHARGINGINFO));
	OSMutexPost (FramMutex);
}
