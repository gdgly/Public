#ifndef _ELMTASK_H
#define _ELMTASK_H
#include "sys.h"

typedef struct
{
	volatile uint8_t  State;					// ״̬
	volatile uint32_t Totalelc;   		// ��ǰ�ܵ���  1/100KWH
	volatile uint32_t RawTotalelc;  	//	ԭʼ�Ĵ�������
	volatile uint32_t Vol;			  		// ��ѹ	1/10 v
	volatile uint32_t RawVol;		  	// ԭʼ�Ĵ�����ѹ
	volatile uint32_t Cur;		  			// ����   1/1000 A
	volatile uint32_t RawCur;		  	//  ԭʼ�Ĵ�������
	volatile uint32_t Kw;		  			// ԭʼԭʼ����
	volatile uint32_t RawKw;		  		//  ����
}ELMINFO;








//���������Ĵ���
#define MPREG_SPLI1             0x00 //����ͨ��1��ADC��������
#define MPREG_SPLI2             0x01 //����ͨ��2��ADC��������
#define MPREG_SPLU              0x02 //��ѹͨ����ADC��������
#define MPREG_IDC               0x03 //Iͨ��ֱ����ֵ
#define MPREG_UDC               0x04 //Uͨ��ֱ����ֵ
#define MPREG_RMSI1             0x06 //����ͨ��1����Чֵ
#define MPREG_RMSI2             0x07 //����ͨ��2����Чֵ
#define MPREG_RMSU              0x08 //��ѹͨ������Чֵ
#define MPREG_FREQU             0x09 // ��ѹƵ��          
#define MPREG_POWERP1           0x0A //��һͨ���й�����
#define MPREG_POWERQ1           0x0B //��һͨ���޹�����
#define MPREG_POWERS            0x0C //���ڹ���
#define MPREG_ENERGYP           0x0D //�й�����
#define MPREG_ENERGYQ           0x0E //�޹�����
#define MPREG_UDETCNT           0x0F //SAG/Peak��������ʱ�����
#define MPREG_POWERP2           0x10 //�ڶ�ͨ���й�����
#define MPREG_POWERQ2           0x11 //�ڶ�ͨ���޹�����
#define MPREG_MAXUWAVE          0x12 //��ѹ�벨���η�ֵ�Ĵ���
#define MPREG_CRCCHECKSUM       0x15 //У�����У��ͼĴ���(CRC16)
#define MPREG_BACKUPDATA        0x16 // ͨѶ���ݱ��ݼĴ���
#define MPREG_COMCHECKSUM       0x17 //ͨѶУ��ͼĴ���
#define MPREG_SUMCHECKSUM       0x18 //У�����У��ͼĴ���
#define MPREG_EMUSR             0x19 //EMU״̬�Ĵ���
#define MPREG_SYSSTA            0x1A //ϵͳ״̬�Ĵ���
#define MPREG_CHIPID            0x1B //ChipID��Ĭ��ֵΪ7053B0
#define MPREG_DEVICEID          0x1C //DeviceID��Ĭ��ֵΪ705321

//У������Ĵ���

#define ECADR_EMUIE               0x30 //�ж�ʹ�ܼĴ���
#define ECADR_EMUIF               0x31 //EMU�ж� ��־�Ĵ���
#define ECADR_WPREG               0x32 //д�����Ĵ���
#define ECADR_SRST								0x33 //�����λ�Ĵ���
#define ECADR_EMUCFG              0x40 //EMU���üĴ���
#define ECADR_FREGCFG             0x41 //ʱ��/����Ƶ�����üĴ���
#define ECADR_MODULEEN            0x42 //EMUģ��ʹ�ܼĴ���
#define ECADR_ANAEN               0x43 //ADC���ؼĴ���
#define ECADR_IOCFG               0x45 //IO������üĴ���
#define ECADR_GP1                 0x50 //ͨ��1���й�����У��
#define ECADR_GQ1                 0x51 //ͨ��1���޹�����У��
#define ECADR_GS1                 0x52 //ͨ��1�����ڹ���У��

#define ECADR_GP2                 0x54 //ͨ��2���й�����У��
#define ECADR_GQ2                 0x55 //ͨ��2���޹�����У��
#define ECADR_GS2                 0x56 //ͨ��2�����ڹ���У��
#define ECADR_QPHSCAL             0x58 //�޹���λ����
#define ECADR_ADCCON              0x59 //ADCͨ������
#define ECADR_I2GAIN              0x5B //����ͨ��2���油��
#define ECADR_I1OFF               0x5C //����ͨ��1��ƫ��У��
#define ECADR_I2OFF               0x5D //����ͨ��2��ƫ��У��
#define ECADR_UOFF                0x5E //��ѹͨ����ƫ��У��
#define ECADR_PQSTART             0x5F //�𶯹�������
#define ECADR_HFCONST             0x61 //������幦������
#define ECADR_DEC_SHIFT           0x64 //ͨ��1����λУ��
#define ECADR_P1OFFSETH           0x65 //ͨ��1�й���������У��������8λ��PIOOFFSETL��ɵ�16bit����
#define ECADR_P2OFFSETH           0x66 //ͨ��2�й���������У��������8λ��PIOOFFSETL��ɵ�16bit����
#define ECADR_Q1OFFSETH           0x67 //ͨ��1�޹���������У��������8λ��PIOOFFSETL��ɵ�16bit����
#define ECADR_Q2OFFSETH           0x68 //ͨ��2�޹���������У��������8λ��PIOOFFSETL��ɵ�16bit����
#define ECADR_I1RMSOFFSET         0x69 //ͨ��1��Чֵ�����Ĵ�����Ϊ16bit�޷�����
#define ECADR_I2RMSOFFSET         0x6A //ͨ��2��Чֵ�����Ĵ�����Ϊ16bit�޷�����
#define ECADR_URMSOFFSET          0x6B //ͨ��U��Чֵ�����Ĵ�����Ϊ16bit�޷�����
#define ECADR_ZCROSSCURRENT       0x6C //����������ֵ���üĴ���
#define ECADR_GPHS1               0x6D //ͨ��1����λУ����PQ��ʽ��               
#define ECADR_GPHS2               0x6E //ͨ��2����λУ����PQ��ʽ��
#define ECADR_PFCNT               0x6F //�����й��������              
#define ECADR_QFCNT               0x70 //�����޹��������
#define ECADR_ANACON              0x72 //ģ����ƼĴ���
#define ECADR_SUMCHECKL           0x73 //У��͵�16λ�����û�д�룬ʹ�ܱȽϹ��ܺ�оƬ�Ƚϸ�����־
#define ECADR_SUMCHECKH           0x74 //����͸�8λ�����û�д�룬ʹ�ܱȽϹ��ܺ�оƬ�Ƚϸ�����־
#define ECADR_MODECFG             0x75 //ģʽ���üĴ���
#define ECADR_P1OFFSETL           0x76 //ͨ��1�й�����ƫ��У��������8λ����P1OFFSETH���16bit ����
#define ECADR_P2OFFSETL           0x77 //ͨ��2�й�����ƫ��У��������8λ����P2OFFSETH���16bit ���� 
#define ECADR_Q1OFFSETL           0x78 //ͨ��1�޹�����ƫ��У��������8λ����Q1OFFSETH���16bit ����
#define ECADR_Q2OFFSETL           0x79 //ͨ��2�޹�����ƫ��У��������8λ����Q2OFFSETH���16bit ����
#define ECADR_UPEAKLV1            0x7A //UPEAK��ֵ�Ĵ�����16λ�޷���������ADC����ֵ�ĸ�λ����
#define ECADR_USAGLV1             0x7B //USAG��ֵ�Ĵ�����16λ�޷���������ADC����ֵ�ĸ�λ����
#define ECADR_UCYLEN              0x7C //PEAK SAG����������üĴ�����16bit

#define HEAD                      0x6A  //֡ͷ




uint8_t 	ElmGetCommState(void);

uint32_t 	ElmGetTotalElc(uint8_t IsInit);
uint32_t 	ElmGetVolA(void);
uint32_t 	ElmGetCurA(void);
uint32_t 	ElmGetKw(void);

uint32_t 	ElmGetRawTotalElc(void);
uint32_t 	ElmGetRawVolA(void);
uint32_t 	ElmGetRawCurA(void);
uint32_t 	ElmGetRawKw(void);

void ElmTask(void *pdata);

#endif




