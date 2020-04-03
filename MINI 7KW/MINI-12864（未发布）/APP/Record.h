#ifndef _RECORD_H
#define _RECORD_H
#include "sys.h"
#include "Cfg.h"
#include "Maintask.h"

//充电记录总条数
#define RECORD_TOTAL_NUM           70
//锁卡记录总条数
#define LOCKCARD_TOTAL_NUM         30




//锁卡信息 预256个字节空间
#define FRAMADDR_LOCKCARDINFO	356

//充电记录信息 预留256个字节空间
#define FRAMADDR_CHARGERECORDINFO	612

//充电过程中记录信息
#define FRAMADDR_CHARGINGRECORDINFO	868

//充电记录信息 预留 7K 空间
#define FRAMADDR_CHARGERECORDDATA	1024

//充电记录size  预留 140K  空间
#define CHARGERECORDDATASIZE  140


//充电记录索引信息
typedef struct{
	uint8_t index;//索引号
	uint16_t Flag;  //标识 固定0x5a5a
	uint8_t recordstate[RECORD_TOTAL_NUM];//记录状态 0无记录 1有记录
	
}CHARGERECORDINFO;

//锁卡记录信息
typedef struct{
	uint8_t StorePoint;//存储指针 0-199
	uint8_t SerialPoint;//序列号指针
	uint8_t SerialNum[LOCKCARD_TOTAL_NUM];//序列号
	uint16_t Flag; //标识 固定0x5a5a
	uint32_t CardID[LOCKCARD_TOTAL_NUM];//卡号
}LOCKCARDINFO;

//充电过程中记录信息
typedef struct{
	uint16_t Head;//固定为0x5a5a
	CHARGINGDATA ChgDat; //需考虑断电时也保存好
	uint16_t Tail;//从头标志（包含头标志）到尾标志（不包含尾标志）的数据累加和
}CHARGINGINFO;

uint32_t GetChargeRecordIndex(uint8_t port);
uint32_t GetChargeRecordNumber(uint8_t port);

uint8_t InsertChargeRecord(CHARGINGDATA * chgdata);
void DeleteChargeRecord(uint16_t index);
uint8_t FindChargeRecord(uint16_t index,CHARGINGDATA * chgdata);

uint32_t InsertLockCardRecord(uint32_t CardID);
void DeleteLockCardRecord(uint32_t CardID);
uint8_t FindLockCardRecord(uint32_t CardID, uint32_t *SerialNum);

uint8_t InsertChargingRecord(CHARGINGDATA * chgdata);
void DeleteChargingRecord(uint8_t port);

void RecordInit(void);
void DeleteAllRecord(void);


#endif


/******************************************

		8K eprom
		地址							说明										大小
		0 -	7  					升级标识相关						8 byte
		8	-	354					配置相关参数						348 byte
		355 - 611				锁卡信息相关						256 byte
		612 - 867 			充电记录索引信息				256 byte
		868 - 1023  		充电过程中记录信息			156 byte
		1028 - 1024*8-1 充电记录								每条 140 byte

*******************************************/
