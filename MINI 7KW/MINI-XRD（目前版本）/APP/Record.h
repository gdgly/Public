#ifndef _RECORD_H
#define _RECORD_H
#include "sys.h"
#include "Cfg.h"
#include "Maintask.h"

//����¼������
#define RECORD_TOTAL_NUM           70
//������¼������
#define LOCKCARD_TOTAL_NUM         30




//������Ϣ Ԥ256���ֽڿռ�
#define FRAMADDR_LOCKCARDINFO	356

//����¼��Ϣ Ԥ��256���ֽڿռ�
#define FRAMADDR_CHARGERECORDINFO	612

//�������м�¼��Ϣ
#define FRAMADDR_CHARGINGRECORDINFO	868

//����¼��Ϣ Ԥ�� 7K �ռ�
#define FRAMADDR_CHARGERECORDDATA	1024

//����¼size  Ԥ�� 140K  �ռ�
#define CHARGERECORDDATASIZE  140


//����¼������Ϣ
typedef struct{
	uint8_t index;//������
	uint16_t Flag;  //��ʶ �̶�0x5a5a
	uint8_t recordstate[RECORD_TOTAL_NUM];//��¼״̬ 0�޼�¼ 1�м�¼
	
}CHARGERECORDINFO;

//������¼��Ϣ
typedef struct{
	uint8_t StorePoint;//�洢ָ�� 0-199
	uint8_t SerialPoint;//���к�ָ��
	uint8_t SerialNum[LOCKCARD_TOTAL_NUM];//���к�
	uint16_t Flag; //��ʶ �̶�0x5a5a
	uint32_t CardID[LOCKCARD_TOTAL_NUM];//����
}LOCKCARDINFO;

//�������м�¼��Ϣ
typedef struct{
	uint16_t Head;//�̶�Ϊ0x5a5a
	CHARGINGDATA ChgDat; //�迼�Ƕϵ�ʱҲ�����
	uint16_t Tail;//��ͷ��־������ͷ��־����β��־��������β��־���������ۼӺ�
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
		��ַ							˵��										��С
		0 -	7  					������ʶ���						8 byte
		8	-	354					������ز���						348 byte
		355 - 611				������Ϣ���						256 byte
		612 - 867 			����¼������Ϣ				256 byte
		868 - 1023  		�������м�¼��Ϣ			156 byte
		1028 - 1024*8-1 ����¼								ÿ�� 140 byte

*******************************************/
