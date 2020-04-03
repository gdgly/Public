#ifndef __CHGTASK_H_
#define __CHGTASK_H_
#include "sys.h"
#define OPEN   0x01
#define CLOSE  0x00

#define CAUSE_USER_NORMAL_STOP 										0x00//�û�����ֹͣ
#define CAUSE_WAIT_INSERTGUN_TIMEOUT							0x01//�ȴ���ǹ��ʱ
#define CAUSE_LOCK_GUN_FAILED											0x02//��ǹʧ��
#define CAUSE_K1K2_ADHESION_FAULT									0x03//�̵���ճ������
#define CAUSE_K1K2_CLOSE_FAULT										0x04//�̵����ܶ�����
#define CAUSE_K1K2_OUTSIDE_VOL_GREATER_THAN_10V		0x05//��Ե���ǰK1K2����ѹ����10V
#define CAUSE_CHARGEMODULE_OUTPUT_FAULT						0x06//���ģ���������
#define CAUSE_INSULATION_DETECT_FAULT							0x07//��Ե����쳣
#define CAUSE_BLEED_UNIT_FAULT										0x08//й�ŵ�·�쳣
#define CAUSE_BMS_PARA_CANNOT_FIT									0x09//�����������ʺ�
#define CAUSE_START_VOLTAGE_FAULT									0x0A//����ʱ��ѹ�쳣
#define CAUSE_BATTERY_READY_TO_NOREADY						0x0B//���׼��������Ϊδ����
#define CAUSE_SUSPEND_TIMEOUT											0x0C//���׮�������ͣ��ʱ
#define CAUSE_CC1_FAULT														0x0D//CC1�쳣
#define CAUSE_BMS_COMMUNICATE_TIMEOUT							0x0E//BMSͨ�ų�ʱ
#define CAUSE_SCRAMSTOP_FAULT											0x0F//��ͣ��������
#define CAUSE_DOOR_OPEN_FAULT											0x10//�Ž�����
#define CAUSE_GUNPORT_OVERTEMP										0x11//���ӿڹ���
#define CAUSE_OUPUTVOLTAGE_OVER_FAULT							0x12//ֱ�������ѹ��ѹ����
#define CAUSE_DEMANDVOL_FAULT											0x13//�����ѹ�쳣
#define CAUSE_BSM_SINGLEBATTERY_OVERVOL_FAULT			0x14//BSM�����е��嶯�����ص�ѹ����
#define CAUSE_BSM_SINGLEBATTERY_LESSVOL_FAULT			0x15//BSM�����е��嶯�����ص�ѹ����
#define CAUSE_BSM_SOC_OVER_FAULT									0x16//BSM������SOC����
#define CAUSE_BSM_SOC_LESS_FAULT									0x17//BSM������SOC����
#define CAUSE_BSM_SINGLEBATTERY_OVERCUR_FAULT			0x18//BSM�����е��嶯�����ص�������
#define CAUSE_BSM_TEMP_OVER_FAULT									0x19//BSM�����ж��������¶ȹ���
#define CAUSE_BSM_INSULATION_FAULT								0x1A//BSM�����ж������ؾ�Ե״̬�쳣
#define CAUSE_BSM_OUTPUTCONNECTER_FAULT						0x1B//BSM�����ж����������������������״̬�쳣
#define CAUSE_BST_NORMAL_REACHSOC									0x1C//BST�����дﵽ�������SOCĿ��ֵ
#define CAUSE_BST_NORMAL_REACHTOTALVOL						0x1D//BST�����дﵽ�ܵ�ѹ���趨ֵ
#define CAUSE_BST_NORMAL_REACHSINGLEVOL						0x1E//BST�����дﵽ�����ѹ���趨ֵ
#define CAUSE_BST_FAULT_INSULATION								0x1F//BST�����о�Ե����
#define CAUSE_BST_FAULT_OUTPUTCONNECTER_OVERTEMP	0x20//BST������������������¹���
#define CAUSE_BST_FAULT_ELEMENT_OVERTEMP					0x21//BST������BMSԪ�����������������
#define CAUSE_BST_FAULT_OUTPUTCONNECTER						0x22//BST�����г������������
#define CAUSE_BST_FAULT_BATTERYOVERTEMP						0x23//BST�����е�����¶ȹ��߹���
#define CAUSE_BST_FAULT_RELAY											0x24//BST�����и�ѹ�̵�������
#define CAUSE_BST_FAULT_CHECKPOINT2								0x25//BST�����м���2��ѹ������
#define CAUSE_BST_FAULT_OTHER											0x26//BST��������������
#define CAUSE_BST_ERROR_CUR												0x27//BST�����е�������
#define CAUSE_BST_ERROR_VOL												0x28//BST�����е�ѹ����
#define CAUSE_CP_FAULT														0x29//CP�쳣
#define CAUSE_CC_FAULT														0x2A//CC�쳣
#define CAUSE_GET_SOURCE_FAILED                   0x2B//�����Դ����ʧ��
#define CAUSE_DC_OVERCUR_FAULT                    0x2C//ֱ���������
#define CAUSE_AC_OVERVOL_FAULT                    0x2D//���������ѹ
#define CAUSE_AC_LESSVOL_FAULT                    0x2E//�������Ƿѹ
#define CAUSE_AC_OVERCUR_FAULT                    0x2F//��������
#define CAUSE_AC_LESSCUR_FAULT                    0x30//����Ƿ��
#define CAUSE_MONEY_FAULT                         0x31//�˻�����
#define CAUSE_MODULE_OVERVOL_FAULT                0x32//���ģ�������ѹ
#define CAUSE_MODULE_LESSVOL_FAULT                0x33//���ģ������Ƿѹ
#define CAUSE_ELM_FAULT                           0x34//������
#define CAUSE_PAUSE_TIMES_FAULT                   0x35//����г����ͣ��������
#define CAUSE_DC_LESSCUR_FAULT                    0x36//ֱ�����Ƿ��

typedef struct{
	//ʵʱ����
	uint8_t HighVol;//�����ѹ
	uint8_t LowVol;//����Ƿѹ
	uint8_t ScramState; //��ͣ״̬ 0���� 1����
	uint8_t GunSeatState;//���׮���ǹ��״̬ 0δ���� 1������
	uint8_t ChargeState; //���״̬ 0���� 1���
	uint16_t VolA;//������A���ѹ ����0.1V
	uint16_t CurA;//������A����� ����0.01A
}CHARGEINFO;

void InitChgGpio(void);
#define SWITCH_ACN() PEout(9)	// PE9
#define SWITCH_AC()  PEout(10)	// PE9
#define StopButton() PEin(7)

void SetDialIndicatorMode(void);
uint8_t TestStop(void);
CHARGEINFO GetChgInfo(void);
uint8_t GetStopCause(void);
uint8_t StartCharge(void);
uint8_t StopCharge(void);
void ChargeTask(void * pdata);
#endif


