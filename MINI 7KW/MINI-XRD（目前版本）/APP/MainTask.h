#ifndef _MAINTASK_H
#define _MAINTASK_H
#include "sys.h"

#ifndef  FALSE
#define  FALSE                     0
#endif

#ifndef  TRUE
#define  TRUE                      1
#endif


//ֹͣ���ԭ��
static const char *stopcausetab[] = {
	"����ֹͣ",//0x00
	"",//0x01
	"",//0x02
	"",//0x03
	"",//0x04
	"",//0x05
	"",//0x06
	"",//0x07
	"",//0x08
	"",//0x09
	"",//0x0A
	"",//0x0B
	"�����ͣ��ʱ",//0x0C
	"",//0x0D
	"",//0x0E
	"��ͣ��������",//0x0F
	"",//0x10
	"",//0x11
	"",//0x12
	"",//0x13
	"",//0x14
	"",//0x15
	"",//0x16
	"",//0x17
	"",//0x18
	"",//0x19
	"",//0x1A
	"",//0x1B
	"",//0x1C
	"",//0x1D
	"",//0x1E
	"",//0x1F
	"",//0x20
	"",//0x21
	"",//0x22
	"",//0x23
	"",//0x24
	"",//0x25
	"",//0x26
	"",//0x27
	"",//0x28
	"���Ƶ����쳣",//0x29
	"���ǹ�����쳣",//0x2A
	"�����Դ����ʧ��",//0x2B
	"ֱ���������",//0x2C
	"�����ѹ��ѹ",//0x2D
	"�����ѹǷѹ",//0x2E
	"����������",//0x2F
	"������������С���Զ�ֹͣ���",//0x30
	"����",//0x31
	"���ģ�������ѹ",//0x32
	"���ģ������Ƿѹ",//0x33
	"������",//0x34
	"����г����ͣ��������",//0x35
	"������������С���Զ�ֹͣ���"// 0x36
};
#define PORTA     0
#define PORTB     1
#define LINENUM     5       //��ʾ����¼������
//��ǹ
#define A_IDLE		0x00 //Aǹ����
#define A_CHARGE	0x01 //Aǹ���
#define A_STOP		0x02 //Aǹֹͣ



#define FAULTCODE_CHARGEBOARD_COMM	1 //������ư�ͨ�Ź���
#define FAULTCODE_SCRAM							2 //��ͣ����
#define FAULTCODE_DOOR							3 //�Ž�����
#define FAULTCODE_METER_COMM				4 //����ͨ�Ź���
#define FAULTCODE_EXPIRE_COMM				5 //ʹ�����޵��ڹ���
#define FAULTCODE_CCBADDRCONFLICT		6 //�����ư��ַ��ͻ����   
#define FAULTCODE_TEMP_ERR          7 //���ǹ������
#define FAULTCODE_HIGHVOL           8 //���������ѹ
#define FAULTCODE_LOWVOL            9 //��������Ƿѹ

#define UNLOCKCARDFAILCODE_WRITECARD	1 //д��ʧ��
#define UNLOCKCARDFAILCODE_NORECORD		2 //��׮û��������¼

#define STARTCHARGETYPE_OFFLINECARD		0 //���߿�
#define STARTCHARGETYPE_ONLINECARD		3 //���߿�
#define STARTCHARGETYPE_PLATFORM			1 //ƽ̨

typedef struct{
	uint32_t StartTimer;//��ʼʱ��ʱ��ֵ
	uint32_t StopTimer;//����ʱ��ʱ��ֵ	
	uint32_t VoltageA;//A���ѹֵ ����0.1V
	uint32_t CurrentA;//A�����ֵ ����0.01A
}GUNINFO;

typedef struct{
	uint8_t StartType;//������ʽ 1���߿� 2���߿� 3��̨����
	uint8_t Mode;//ģʽ 0���� 1������ 2��ʱ�� 3�����
	uint8_t StartDateTime[6];//�������� hex [0]year [1]month [2]day [3]hour [4]min [5]sec
	uint8_t StopDateTime[6];//ֹͣ���� hex [0]year [1]month [2]day [3]hour [4]min [5]sec
	uint8_t StopCause;//ֹͣԭ��
	uint8_t SerialNum;//���к�
	uint16_t Flag;//����¼�̶���ʶ 0x5a5a
	uint16_t PeriodEnergy[48]; //ʱ�ε��� ����0.01��
	uint16_t StartMeterEnergy;//��ʼ������ʾֵ ����0.01��
	uint16_t StopMeterEnergy;//����������ʾֵ ����0.01��
	uint16_t SumEnergy;//�ۼƳ����� ����0.01��
	uint16_t SumMoney;//�ۼƳ���� ����0.01Ԫ	
	uint32_t StartCardID;//ˢ������ʱ����������
	uint32_t StartCardMoney;//���ǰ����� ����0.01Ԫ
	uint32_t Para;//���� ������ʱ����Ϊ0.01Ԫ ������ʱ����Ϊ0.01kWh ��ʱ��ʱ����Ϊ�� �����ʱ����Ϊ0.01Ԫ
	uint32_t SumTime;//�ۼƳ��ʱ�� ����1����
}CHARGINGDATA;


typedef struct 
{
	GUNINFO GunInfo;
	CHARGINGDATA ChgDat; //�迼�Ƕϵ�ʱҲ�����
}MAININFO;

void  	MainTask(void *pdata);

#define MAIN_IDEL			0 //����
#define MAIN_CHARGE		1 //���
#define MAIN_COMPLETE	2 //���

#define SET_START_CHARGE	1
#define SET_STOP_CHARGE 	2

#define STATE_IDLE			0
#define STATE_CHARGE		1
#define STATE_COMPLETE	2

void SetUpdataSucFlag(void);
uint8_t GetLifeTime(void);
uint8_t GetStartingChg(uint8_t port);
uint8_t GetStartType(uint8_t port);
uint32_t GetStartCardID(uint8_t port);
uint8_t GetStartMode(uint8_t port);
uint32_t GetStartPara(uint8_t port);
void GetVoltage(uint8_t port, uint32_t *VolA, uint32_t *VolB, uint32_t *VolC);
void GetCurrent(uint8_t port, uint32_t *CurA, uint32_t *CurB, uint32_t *CurC);

uint32_t GetStartMeterEnergy(uint8_t port);
uint32_t GetStopMeterEnergy(uint8_t port);
uint32_t GetSumEnergy(uint8_t port);
uint32_t GetSumMoney(uint8_t port);
uint16_t GetPeriodEnergy(uint8_t port, uint8_t Period);
uint32_t GetSumTime(uint8_t port);
void GetStartDateTime(uint8_t port, uint8_t *datetime);
void GetStopDateTime(uint8_t port, uint8_t *datetime);
uint32_t GetStartCardMoney(uint8_t port);
void GetCardOrDiscernNumber(uint8_t port, uint8_t *CardOrDiscernNumber);
void SetMainCharge(uint8_t port, uint8_t setcmd, uint8_t mode, uint32_t para, uint8_t *CardOrDiscernNumber, uint8_t *userid);
uint8_t GetMainCharge(uint8_t port);
uint8_t GetChargeRecord(uint8_t port, CHARGINGDATA* ChgData);

#endif
