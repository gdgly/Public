#include "xrd.h"
#include "MyAlgLib.h"
#include "MainTask.h"
#include "Record.h"
#include "DisPlay.h"
#include "Para.h"
#include "usart.h"
/************************************************************************************************************
** �� �� �� : DispMenuInfo
** �������� : ��ʾ�˵������Ϣ
** ��    �� : Time  ʱ��ָ��  Online  ���߱�ʶ
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispMenuInfo(uint8_t* Time, uint8_t* Online)
{

	XRD_Write_Addr(PAGE_All_TIME, (char *)Time, PAGE_All_TIME_LEN);//ʱ��
	XRD_Write_Addr(PAGE_ON_LINEFLAG, (char *)Online, PAGE_ON_LINEFLAG_LEN);//���߱�־
}


/************************************************************************************************************
** �� �� �� : DispStartDeviceInfo
** �������� : ��ʾ�豸�����н���
** ��    �� : MajorVer---���汾��  MinorVer ---�ΰ汾��     PatchVer ---�޶���
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispStartDeviceInfo(uint8_t MajorVer, uint8_t MinorVer, uint8_t PatchVer)
{
	uint8_t tempbuf[64]={0};
	uint32_t day;
	sprintf((char *)tempbuf, "VER:%d.%d.%d",MajorVer, MinorVer, PatchVer);
	XRD_Write_Addr(PAGE0_VER_ADDR, (char *)tempbuf, PAGE0_VER_LEN);
	XRD_Write_Addr(PAGE0_DAY_ADDR, (char *)"    ", PAGE0_DAY_ADDR_LEN );
	if(ParaGetLimitHour()){
		if(ParaGetRunHour() >= ParaGetLimitHour()){
			sprintf((char *)tempbuf, "�뼤��");
		}else{
			day = (ParaGetLimitHour() -ParaGetRunHour()) /24;
			sprintf((char *)tempbuf, "ʹ��ʣ��%d��", day);
		}
		XRD_Write_Addr(PAGE0_DAY_ADDR, (char *)tempbuf, PAGE0_DAY_ADDR_LEN );
	}
	
	XRD_Load_Page(PAGE0);
}




/************************************************************************************************************
** �� �� �� : DispErrIdGunInfo
** �������� : ��ʾǹ����ID
** ��    �� : port  AǹBǹ   IDBuf ����IDָ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispErrIdGunInfo(uint8_t port, uint8_t* IdBuf)
{
	if(port == 0){
		XRD_Write_Addr(PAGE_A_ERRID, (char *)IdBuf, PAGE_A_ERRID_LEN);
	}else {
		XRD_Write_Addr(PAGE_B_ERRID, (char *)IdBuf, PAGE_B_ERRID_LEN);
	}
}
	






/************************************************************************************************************
** �� �� �� : DispAccount
** �������� : ��ʾ�������
** ��    �� : SumEnergy    ������           0.01kwh
**						SumMoney     ���ѽ��           0.01Ԫ
**            RemainMoney   ���              0.01Ԫ  
**            SumTime      ���ʱ��           S  ������
**            Date          ����ָ��        
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAccount(uint32_t SumEnergy, uint32_t SumMoney, uint32_t RemainMoney, uint32_t SumTime, uint8_t* Date)
{
	uint8_t tempbuf[128]={0};
	
	sprintf((char *)tempbuf, "%d.%02d", SumEnergy / 100, SumEnergy % 100);
	XRD_Write_Addr(PAGE13_AB_POWER, (char *)tempbuf, PAGE13_AB_POWER_LEN);//AB��ǰ����	
	sprintf((char *)tempbuf, "%d.%02d", SumMoney / 100, SumMoney % 100);
	XRD_Write_Addr(PAGE13_AB_COST, (char *)tempbuf, PAGE13_AB_COST_LEN);//�������ѽ��
	sprintf((char *)tempbuf, "%d.%02d", RemainMoney / 100, RemainMoney % 100);
	XRD_Write_Addr(PAGE13_AB_REMAIN, (char *)tempbuf, PAGE13_AB_REMAIN_LEN);//���
	sprintf((char *)tempbuf, "%d Min", SumTime / 60);
	XRD_Write_Addr(PAGE13_AB_CH_TIME, (char *)tempbuf, PAGE13_AB_CH_TIME_LEN);//�ܺ�ʱ
	sprintf((char *)tempbuf, "20%02d/%02d/%02d %02d:%02d:%02d", Date[0], Date[1], Date[2], Date[3], Date[4],Date[5]);
	XRD_Write_Addr(PAGE13_AB_DATE, (char *)tempbuf, PAGE13_AB_DATE_LEN);//����
	XRD_Load_Page(PAGE13);
}



/************************************************************************************************************
** �� �� �� : DispDeviceFault
** �������� : ��ʾ��׮���Ͻ���
** ��    �� : tempbuf    ����ID buf
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispInsertGunInfo(void)
{
		XRD_Load_Page(PAGE14);
}



/************************************************************************************************************
** �� �� �� : DispDeviceFault
** �������� : ��ʾ��׮���Ͻ���
** ��    �� : tempbuf    ����ID buf
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispDeviceFault(uint8_t* tempbuf)
{
	
	XRD_Write_Addr(PAGE15_AB_ERR,(char *)tempbuf,PAGE15_AB_ERR_LEN);
	XRD_Load_Page(PAGE15); 
}




/************************************************************************************************************
** �� �� �� : DispCardLockInfo
** �������� : ��ʾ��������
** ��    �� : Pile    ׮����ַ���
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispCardLockInfo(uint8_t* Pile )
{
	XRD_Write_Addr(PAGE16_AB_LOCK, (char *)Pile, PAGE16_AB_LOCK_LEN);	
	XRD_Load_Page(PAGE16); 			
}



/************************************************************************************************************
** �� �� �� : DispUnLockFailureInfo
** �������� : ��ʾ����ʧ�ܽ���
** ��    �� : FailureId    ʧ��ID
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispUnLockFailureInfo(uint32_t FailureId )
{
	uint8_t tempbuf[56]={0};
	
	sprintf((char *)tempbuf, "%02d ", FailureId);
	XRD_Write_Addr(PAGE17_AB_FAILURE_ID,(char *)tempbuf, PAGE17_AB_FAILURE_ID_LEN);	
	XRD_Load_Page(PAGE17); 				
}


/************************************************************************************************************
** �� �� �� : DispNoPileUsed
** �������� : ��ʾ�޿��г��λ
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispNoPileUsed(void)
{
	XRD_Load_Page(PAGE18); 				
}



/************************************************************************************************************
** �� �� �� : DispRemainMoneyInfo
** �������� : ��ʾ������
** ��    �� : Money     0.01Ԫ
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispRemainMoneyInfo(uint32_t Money)
{
	uint8_t tempbuf[56]={0};
	
	sprintf((char *)tempbuf,"%d.%02d", Money/100, Money%100);
	XRD_Write_Addr(PAGE19_AB_CURRENT_MONEY, (char *)tempbuf, PAGE19_AB_CURRENT_MONEY_LEN);
	XRD_Load_Page(PAGE19); 
}
	


/************************************************************************************************************
** �� �� �� : DispInVerify
** �������� : ��ʾ��֤�н���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispInVerify(void)
{
	XRD_Load_Page(PAGE20);
}



/************************************************************************************************************
** �� �� �� : DispVerifySuccesInfo
** �������� : ��ʾ��֤�ɹ�����
** ��    �� : Money   ��ǰ���   0.01Ԫ
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispVerifySuccesInfo(uint32_t Money)
{
	uint8_t tempbuf[56]={0};
	
	sprintf((char *)tempbuf,"%d.%02d", Money/100, Money%100);
	XRD_Write_Addr(PAGE21_AB_MONEY, (char *)tempbuf, PAGE21_AB_MONEY_LEN);
	XRD_Load_Page(PAGE21); 
}



/************************************************************************************************************
** �� �� �� : DispVerifyFailure
** �������� : ��ʾ��֤ʧ�ܽ���
** ��    �� : tempbuf   ʧ��ԭ��ָ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispVerifyFailure(uint8_t* tempbuf)
{
	
	XRD_Write_Addr(PAGE22_AB_FAILURE, (char *)tempbuf, PAGE22_AB_FAILURE_LEN);
	XRD_Load_Page(PAGE22);
}


/************************************************************************************************************
** �� �� �� : DispStartChgInfo
** �������� : ��ʾ�����н���
** ��    �� : Time   ����ʱ��    ��λS ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispStartChgInfo(uint32_t Time)
{
	uint8_t tempbuf[56]={0};
	
	sprintf((char *)tempbuf,"%02d", Time);
//	XRD_Write_Addr(PAGE23_TIME, (char *)tempbuf, PAGE23_TIME_LEN);
	XRD_Load_Page(PAGE23); 
}


/************************************************************************************************************
** �� �� �� : DispOperationInfo
** �������� : ��ʾ�����н���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispOperationInfo(void)
{
	XRD_Load_Page(PAGE24); 	
}

/************************************************************************************************************
** �� �� �� : DispStartSucessInfo
** �������� : ��ʾ�����ɹ�����
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispStartSucessInfo(void)
{
	XRD_Load_Page(PAGE25); 
}


/************************************************************************************************************
** �� �� �� : DispStartFailureInfo
** �������� : ��ʾ����ʧ�ܽ���
** ��    �� : ��
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispStartFailureInfo(void)
{
	XRD_Load_Page(PAGE26); 	
}






/************************************************************************************************************
******************************                ��ǹ������             **************************************
*************************************************************************************************************
*/

/************************************************************************************************************
** �� �� �� : DispAIdleInfo
** �������� : ��ʾA��ǹ��������
** ��    �� : ACode   Aǹ��ά��ָ��    APileNum  Aǹ����ַ�ָ��  GunStatus  A��ǹ״̬
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAIdleInfo(uint8_t* ACode, uint8_t* APileNum, uint8_t GunStatus)
{
	
	XRD_Write_Addr(PAGE1_A_DEV_NUM, (char *)APileNum, PAGE1_A_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE1_A_QR, (char *)ACode, PAGE1_A_QR_LEN);	
	XRD_Draw_Ioc(PAGE1_A_CH_FLAG, GunStatus);
	XRD_Load_Page(PAGE1);
}



/************************************************************************************************************
** �� �� �� : DispAChargeInfo
** �������� : ��ʾA��ǹ������
** ��    �� : SumEnergy   A��ǰ����  0.01kwh
**						SumMoney    A��ǰ����  0.01Ԫ
**            CurrentA    A����      0.001 ��3λС��
**            VoltageA    A��ѹ      0.1  V
**            SumTime     A���ʱ��  S  ������
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAChargeInfo(uint32_t SumEnergy, uint32_t SumMoney, uint32_t CurrentA, uint32_t CurrentB, uint32_t CurrentC, uint32_t VoltageA,  uint32_t VoltageB,  uint32_t VoltageC, uint32_t SumTime)
{
	char tempbuf[56]={0};
	sprintf((char *)tempbuf, "%d.%02d", SumEnergy / 100, SumEnergy % 100);
	XRD_Write_Addr(PAGE2_A_POWER, (char *)tempbuf, PAGE2_A_POWER_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", SumMoney / 100, SumMoney % 100);
	XRD_Write_Addr(PAGE2_A_COST, (char *)tempbuf, PAGE2_A_COST_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", CurrentA / 1000, (CurrentA % 1000)/10);
	XRD_Write_Addr(PAGE2_A_I_A, (char *)tempbuf, PAGE2_A_I_LEN);//A����A
	sprintf((char *)tempbuf, "%d.%02d", CurrentB / 1000, (CurrentB % 1000)/10);
	XRD_Write_Addr(PAGE2_A_I_B, (char *)tempbuf, PAGE2_A_I_LEN);//A����B
	sprintf((char *)tempbuf, "%d.%02d", CurrentC / 1000, (CurrentC % 1000)/10);
	XRD_Write_Addr(PAGE2_A_I_C, (char *)tempbuf, PAGE2_A_I_LEN);//A����C	
	sprintf((char *)tempbuf, "%d.%d", VoltageA / 10, VoltageA % 10);
	XRD_Write_Addr(PAGE2_A_VOLTAGE, (char *)tempbuf, PAGE2_A_VOLTAGE_LEN);//A��ѹA
	sprintf((char *)tempbuf, "%d.%d", VoltageB / 10, VoltageB % 10);
	XRD_Write_Addr(PAGE2_A_VOLTAGE_B, (char *)tempbuf, PAGE2_A_VOLTAGE_LEN);//A��ѹB
	sprintf((char *)tempbuf, "%d.%d", VoltageC / 10, VoltageC % 10);
	XRD_Write_Addr(PAGE2_A_VOLTAGE_C, (char *)tempbuf, PAGE2_A_VOLTAGE_LEN);//A��ѹC	
	sprintf((char *)tempbuf, "%02d:%02d:%02d", SumTime / 3600, (SumTime % 3600)/60, ((SumTime % 3600)%60) );
	XRD_Write_Addr(PAGE2_A_CH_TIME, (char *)tempbuf, PAGE2_A_CH_TIME_LEN);//A���ʱ��


	
	XRD_Load_Page(PAGE2);
}




/************************************************************************************************************
** �� �� �� : DispAStopInfo
** �������� : ��ʾA��ǹֹͣ����
** ��    �� : Prompt     	A��ʾ�ַ���ָ��
**						StpReason   Aֹͣԭ���ַ�ָ��
**            SumEnergy   A������          0.01kwh
**            SumMoney    A�����          0.01Ԫ
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAStopInfo(uint8_t* Prompt, uint8_t* StpReason, uint32_t SumEnergy, uint32_t SumMoney )
{
	char tempbuf[56]={0};
	XRD_Write_Addr(PAGE3_A_STOP_REASON, (char *)StpReason, PAGE3_A_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE3_A_CHARG_PROMPT, (char *)Prompt, PAGE3_A_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", SumEnergy / 100, SumEnergy % 100);
	XRD_Write_Addr(PAGE3_A_CHARG_POWER,(char *)tempbuf,PAGE3_A_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", SumMoney / 100, SumMoney % 100);;											
	XRD_Write_Addr(PAGE3_A_CHARG_MONEY,(char *)tempbuf,PAGE3_A_CHARG_MONEY_LEN);	
	XRD_Load_Page(PAGE3); 
}



/************************************************************************************************************
******************************                ˫ǹ������             **************************************
*************************************************************************************************************
*/

/************************************************************************************************************
** �� �� �� : DispAIdleBIdleInfo
** �������� : ��ʾA����B��������
** ��    �� :  ACode   Aǹ��ά��ָ��    APileNum  Aǹ����ַ�ָ��  AGunStatus  A��ǹ״̬  
**          :  BCode   Bǹ��ά��ָ��    BPileNum  Bǹ����ַ�ָ��  BGunStatus  B��ǹ״̬   
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAIdleBIdleInfo( uint8_t* ACode, uint8_t* APileNum, uint8_t AGunStatus, \
							           uint8_t* BCode, uint8_t* BPileNum, uint8_t BGunStatus)
{
	XRD_Write_Addr(PAGE4_A_DEV_NUM, (char *)APileNum, PAGE4_A_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE4_A_QR, (char *)ACode, PAGE4_A_QR_LEN);	
	XRD_Draw_Ioc(PAGE4_A_CH_FLAG, AGunStatus);
	
	XRD_Write_Addr(PAGE4_B_DEV_NUM, (char *)BPileNum, PAGE4_B_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE4_B_QR, (char *)BCode, PAGE4_B_QR_LEN);	
	XRD_Draw_Ioc(PAGE4_B_CH_FLAG, BGunStatus);
	XRD_Load_Page(PAGE4);
}



/************************************************************************************************************
** �� �� �� : DispAChgBIdleInfo
** �������� : ��ʾA���B��������
** ��    �� : ASumEnergy   A��ǰ����   0.01kwh
**						ASumMoney    A��ǰ����   0.01Ԫ
**            ACurrent     A����       0.001 ��3λС��
**            AVoltage     A��ѹ       0.1  V
**            ASumTime     A���ʱ��    S  ������
**            BCode   Bǹ��ά��ָ��    BPileNum  Bǹ����ַ�ָ��  BGunStatus  B��ǹ״̬   
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAChgBIdleInfo( uint32_t ASumEnergy, uint32_t ASumMoney, uint32_t ACurrentA, uint32_t ACurrentB, uint32_t ACurrentC, uint32_t AVoltageA, uint32_t AVoltageB,  uint32_t AVoltageC,   uint32_t ASumTime, \
											  uint8_t* BCode, uint8_t* BPileNum, uint8_t BGunStatus)
{
	char tempbuf[56]={0};
	
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE5_A_POWER, (char *)tempbuf, PAGE5_A_POWER_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);
	XRD_Write_Addr(PAGE5_A_COST, (char *)tempbuf, PAGE5_A_COST_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ACurrentA / 1000, (ACurrentA % 1000)/10);
	XRD_Write_Addr(PAGE2_A_I_A, (char *)tempbuf, PAGE2_A_I_LEN);//A����A
	sprintf((char *)tempbuf, "%d.%02d", ACurrentB / 1000, (ACurrentB % 1000)/10);
	XRD_Write_Addr(PAGE2_A_I_B, (char *)tempbuf, PAGE2_A_I_LEN);//A����B
	sprintf((char *)tempbuf, "%d.%02d", ACurrentC / 1000, (ACurrentC % 1000)/10);
	XRD_Write_Addr(PAGE2_A_I_C, (char *)tempbuf, PAGE2_A_I_LEN);//A����C	
	sprintf((char *)tempbuf, "%d.%d", AVoltageA / 10, AVoltageA % 10);
	XRD_Write_Addr(PAGE2_A_VOLTAGE, (char *)tempbuf, PAGE2_A_VOLTAGE_LEN);//A��ѹA
	sprintf((char *)tempbuf, "%d.%d", AVoltageB / 10, AVoltageB % 10);
	XRD_Write_Addr(PAGE2_A_VOLTAGE_B, (char *)tempbuf, PAGE2_A_VOLTAGE_LEN);//A��ѹB
	sprintf((char *)tempbuf, "%d.%d", AVoltageC / 10, AVoltageC % 10);
	XRD_Write_Addr(PAGE2_A_VOLTAGE_C, (char *)tempbuf, PAGE2_A_VOLTAGE_LEN);//A��ѹC	
	sprintf((char *)tempbuf, "%02d:%02d:%02d", ASumTime / 3600, (ASumTime % 3600)/60, ((ASumTime % 3600)%60) );
	XRD_Write_Addr(PAGE5_A_CH_TIME, (char *)tempbuf, PAGE5_A_CH_TIME_LEN);//A���ʱ��
		
	XRD_Write_Addr(PAGE5_B_DEV_NUM, (char *)BPileNum, PAGE5_B_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE5_B_QR, (char *)BCode, PAGE5_B_QR_LEN);	
	XRD_Draw_Ioc(PAGE5_B_CH_FLAG, BGunStatus);
	XRD_Load_Page(PAGE5);	
}

/************************************************************************************************************
** �� �� �� : DispAStopBIdle
** �������� : ��ʾAǹֹͣB��������
** ��    �� : APrompt      A��ʾ�ַ���ָ��
**						AStpReason   Aֹͣԭ���ַ�ָ��
**            ASumEnergy   A������          0.01kwh
**            ASumMoney    A�����          0.01Ԫ
**            BCode   Bǹ��ά��ָ��    BPileNum  Bǹ����ַ�ָ��  BGunStatus  B��ǹ״̬  
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAStopBIdleInfo( uint8_t* APrompt, uint8_t* AStpReason, uint32_t ASumEnergy, uint32_t ASumMoney, \
										     uint8_t* BCode, uint8_t* BPileNum, uint8_t BGunStatus)
{
	char tempbuf[56]={0};
	
	XRD_Write_Addr(PAGE6_A_STOP_REASON, (char *)AStpReason, PAGE6_A_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE6_A_CHARG_PROMPT, (char *)APrompt, PAGE6_A_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE6_A_CHARG_POWER,(char *)tempbuf, PAGE6_A_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);;											
	XRD_Write_Addr(PAGE6_A_CHARG_MONEY,(char *)tempbuf, PAGE6_A_CHARG_MONEY_LEN);	
	
	XRD_Write_Addr(PAGE6_B_DEV_NUM, (char *)BPileNum, PAGE6_B_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE6_B_QR, (char *)BCode, PAGE6_B_QR_LEN);	
	XRD_Draw_Ioc(PAGE6_B_CH_FLAG, BGunStatus);
	XRD_Load_Page(PAGE6);	
}




/************************************************************************************************************
** �� �� �� : DispAIdleBChgInfo
** �������� : ��ʾA����B����н���
** ��    �� :  ACode   Aǹ��ά��ָ��    APileNum  Aǹ����ַ�ָ��  AGunStatus  A��ǹ״̬  
**             BSumEnergy   B��ǰ����   0.01kwh
**						 BSumMoney    B��ǰ����   0.01Ԫ
**             BCurrent     B����       0.001 ��3λС��
**             BVoltage     B��ѹ       0.1  V
**             BSumTime     B���ʱ��    S  ������  
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAIdleBChgInfo( uint8_t* ACode, uint8_t* APileNum, uint8_t AGunStatus,  \
												uint32_t BSumEnergy, uint32_t BSumMoney, uint32_t BCurrentA, uint32_t BCurrentB,uint32_t BCurrentC,uint32_t BVoltageA, uint32_t BVoltageB, uint32_t BVoltageC,  uint32_t BSumTime )
{
	char tempbuf[56]={0};
	
	XRD_Write_Addr(PAGE7_A_DEV_NUM, (char *)APileNum, PAGE7_A_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE7_A_QR, (char *)ACode, PAGE7_A_QR_LEN);	
	XRD_Draw_Ioc(PAGE7_A_CH_FLAG, AGunStatus);
	
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE7_B_POWER, (char *)tempbuf, PAGE7_B_POWER_LEN);//B��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);
	XRD_Write_Addr(PAGE7_B_COST, (char *)tempbuf, PAGE7_B_COST_LEN);//B��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", BCurrentA / 1000, (BCurrentA % 1000)/10);
	XRD_Write_Addr(PAGE7_B_I, (char *)tempbuf, PAGE7_B_I_LEN);//B����A
	sprintf((char *)tempbuf, "%d.%02d", BCurrentB / 1000, (BCurrentB % 1000)/10);
	XRD_Write_Addr(PAGE7_B_I_B, (char *)tempbuf, PAGE7_B_I_LEN);//B����B
	sprintf((char *)tempbuf, "%d.%02d", BCurrentC / 1000, (BCurrentC % 1000)/10);
	XRD_Write_Addr(PAGE7_B_I_C, (char *)tempbuf, PAGE7_B_I_LEN);//B����C
	sprintf((char *)tempbuf, "%d.%d", BVoltageA / 10, BVoltageA % 10);
	XRD_Write_Addr(PAGE7_B_VOLTAGE, (char *)tempbuf, PAGE7_B_VOLTAGE_LEN);//B��ѹA
	sprintf((char *)tempbuf, "%d.%d", BVoltageB / 10, BVoltageB % 10);
	XRD_Write_Addr(PAGE7_B_VOLTAGE_B, (char *)tempbuf, PAGE7_B_VOLTAGE_LEN);//B��ѹB
	sprintf((char *)tempbuf, "%d.%d", BVoltageC / 10, BVoltageC % 10);
	XRD_Write_Addr(PAGE7_B_VOLTAGE_C, (char *)tempbuf, PAGE7_B_VOLTAGE_LEN);//B��ѹC
	sprintf((char *)tempbuf, "%02d:%02d:%02d", BSumTime / 3600, (BSumTime % 3600)/60, ((BSumTime % 3600)%60) );
	XRD_Write_Addr(PAGE7_B_CH_TIME, (char *)tempbuf, PAGE7_B_CH_TIME_LEN);//B���ʱ��
	XRD_Load_Page(PAGE7);	
}


/************************************************************************************************************
** �� �� �� : DispAIdleBStop
** �������� : ��ʾA����B���ֹͣ����
** ��    �� :  ACode   Aǹ��ά��ָ��    APileNum  Aǹ����ַ�ָ��  AGunStatus  A��ǹ״̬  
**             BPrompt      B��ʾ�ַ���ָ��
**						 BStpReason   Bֹͣԭ���ַ�ָ��
**             BSumEnergy   B������          0.01kwh
**             BSumMoney    B�����          0.01Ԫ
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAIdleBStopInfo( uint8_t* ACode, uint8_t* APileNum, uint8_t AGunStatus, \
												 uint8_t* BPrompt, uint8_t* BStpReason, uint32_t BSumEnergy, uint32_t BSumMoney )
{
	char tempbuf[56]={0};
	
	XRD_Write_Addr(PAGE8_A_DEV_NUM, (char *)APileNum, PAGE4_B_DEV_NUM_LEN);			
	XRD_Write_Addr(PAGE8_A_QR, (char *)ACode, PAGE8_A_QR_LEN);	
	XRD_Draw_Ioc(PAGE8_A_CH_FLAG, AGunStatus);
	
	XRD_Write_Addr(PAGE8_B_STOP_REASON, (char *)BStpReason, PAGE8_B_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE8_B_CHARG_PROMPT, (char *)BPrompt, PAGE8_B_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE8_B_CHARG_POWER,(char *)tempbuf, PAGE8_B_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);;											
	XRD_Write_Addr(PAGE8_B_CHARG_MONEY,(char *)tempbuf, PAGE8_B_CHARG_MONEY_LEN);	
	XRD_Load_Page(PAGE8);	
	
}










/************************************************************************************************************
** �� �� �� : DispAStopBChgInfo
** �������� : ��ʾA���ֹͣB����н���
** ��    �� :  APrompt      A��ʾ�ַ���ָ��
**						 AStpReason   Aֹͣԭ���ַ�ָ��
**             ASumEnergy   A������          0.01kwh
**             ASumMoney    A�����          0.01Ԫ
**             BSumEnergy   B��ǰ����          0.01kwh
**						 BSumMoney    B��ǰ����          0.01Ԫ
**             BCurrent     B����              0.001 ��3λС��
**             BVoltage     B��ѹ              0.1  V
**             BSumTime     B���ʱ��          S  ������  
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAStopBChgInfo( uint8_t* APrompt, uint8_t* AStpReason, uint32_t ASumEnergy, uint32_t ASumMoney, \
												uint32_t BSumEnergy, uint32_t BSumMoney, uint32_t BCurrentA, uint32_t BCurrentB, uint32_t BCurrentC, uint32_t BVoltageA, uint32_t BVoltageB, uint32_t BVoltageC, uint32_t BSumTime )
{
	char tempbuf[56]={0};
	
	XRD_Write_Addr(PAGE9_A_STOP_REASON, (char *)AStpReason, PAGE9_A_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE9_A_CHARG_PROMPT, (char *)APrompt, PAGE9_A_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE9_A_CHARG_POWER,(char *)tempbuf, PAGE9_A_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);;											
	XRD_Write_Addr(PAGE9_A_CHARG_MONEY,(char *)tempbuf, PAGE9_A_CHARG_MONEY_LEN);	
		
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE9_B_POWER, (char *)tempbuf, PAGE9_B_POWER_LEN);//B��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);
	XRD_Write_Addr(PAGE9_B_COST, (char *)tempbuf, PAGE9_B_COST_LEN);//B��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", BCurrentA / 1000, (BCurrentA % 1000)/10);
	XRD_Write_Addr(PAGE7_B_I, (char *)tempbuf, PAGE7_B_I_LEN);//B����A
	sprintf((char *)tempbuf, "%d.%02d", BCurrentB / 1000, (BCurrentB % 1000)/10);
	XRD_Write_Addr(PAGE7_B_I_B, (char *)tempbuf, PAGE7_B_I_LEN);//B����B
	sprintf((char *)tempbuf, "%d.%02d", BCurrentC / 1000, (BCurrentC % 1000)/10);
	XRD_Write_Addr(PAGE7_B_I_C, (char *)tempbuf, PAGE7_B_I_LEN);//B����C
	sprintf((char *)tempbuf, "%d.%d", BVoltageA / 10, BVoltageA % 10);
	XRD_Write_Addr(PAGE7_B_VOLTAGE, (char *)tempbuf, PAGE7_B_VOLTAGE_LEN);//B��ѹA
	sprintf((char *)tempbuf, "%d.%d", BVoltageB / 10, BVoltageB % 10);
	XRD_Write_Addr(PAGE7_B_VOLTAGE_B, (char *)tempbuf, PAGE7_B_VOLTAGE_LEN);//B��ѹB
	sprintf((char *)tempbuf, "%d.%d", BVoltageC / 10, BVoltageC % 10);
	XRD_Write_Addr(PAGE7_B_VOLTAGE_C, (char *)tempbuf, PAGE7_B_VOLTAGE_LEN);//B��ѹC
	sprintf((char *)tempbuf, "%02d:%02d:%02d", BSumTime / 3600, (BSumTime % 3600)/60, ((BSumTime % 3600)%60) );
	XRD_Write_Addr(PAGE9_B_CH_TIME, (char *)tempbuf, PAGE9_B_CH_TIME_LEN);//B���ʱ��
	XRD_Load_Page(PAGE9);	
}

/************************************************************************************************************
** �� �� �� : DispAChgBStopInfo
** �������� : ��ʾA�����B���ֹͣ����
** ��    �� :  ASumEnergy   A��ǰ����           0.01kwh
**						 ASumMoney    A��ǰ����           0.01Ԫ
**             ACurrent     A����               0.001 ��3λС��
**             AVoltage     A��ѹ               0.1  V
**             ASumTime     A���ʱ��           S  ������
**             BPrompt      B��ʾ�ַ���ָ��
**						 BStpReason   Bֹͣԭ���ַ�ָ��
**             BSumEnergy   B������           0.01kwh
**             BSumMoney    B�����           0.01Ԫ
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAChgBStopInfo( uint32_t ASumEnergy, uint32_t ASumMoney, uint32_t ACurrentA, uint32_t ACurrentB, uint32_t ACurrentC, uint32_t AVoltageA, uint32_t AVoltageB, uint32_t AVoltageC,  uint32_t ASumTime, \
												uint8_t* BPrompt, uint8_t* BStpReason, uint32_t BSumEnergy, uint32_t BSumMoney )
{
	char tempbuf[56]={0};
	
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE10_A_POWER, (char *)tempbuf, PAGE10_A_POWER_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);
	XRD_Write_Addr(PAGE10_A_COST, (char *)tempbuf, PAGE10_A_COST_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ACurrentA / 1000, (ACurrentA % 1000)/10);
	XRD_Write_Addr(PAGE2_A_I_A, (char *)tempbuf, PAGE2_A_I_LEN);//A����A
	sprintf((char *)tempbuf, "%d.%02d", ACurrentB / 1000, (ACurrentB % 1000)/10);
	XRD_Write_Addr(PAGE2_A_I_B, (char *)tempbuf, PAGE2_A_I_LEN);//A����B
	sprintf((char *)tempbuf, "%d.%02d", ACurrentC / 1000, (ACurrentC % 1000)/10);
	XRD_Write_Addr(PAGE2_A_I_C, (char *)tempbuf, PAGE2_A_I_LEN);//A����C	
	sprintf((char *)tempbuf, "%d.%d", AVoltageA / 10, AVoltageA % 10);
	XRD_Write_Addr(PAGE2_A_VOLTAGE, (char *)tempbuf, PAGE2_A_VOLTAGE_LEN);//A��ѹA
	sprintf((char *)tempbuf, "%d.%d", AVoltageB / 10, AVoltageB % 10);
	XRD_Write_Addr(PAGE2_A_VOLTAGE_B, (char *)tempbuf, PAGE2_A_VOLTAGE_LEN);//A��ѹB
	sprintf((char *)tempbuf, "%d.%d", AVoltageC / 10, AVoltageC % 10);
	XRD_Write_Addr(PAGE2_A_VOLTAGE_C, (char *)tempbuf, PAGE2_A_VOLTAGE_LEN);//A��ѹC	
	sprintf((char *)tempbuf, "%02d:%02d:%02d", ASumTime / 3600, (ASumTime % 3600)/60, ((ASumTime % 3600)%60) );
	XRD_Write_Addr(PAGE10_A_CH_TIME, (char *)tempbuf, PAGE10_A_CH_TIME_LEN);//A���ʱ��
	
	XRD_Write_Addr(PAGE10_B_STOP_REASON, (char *)BStpReason, PAGE10_B_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE10_B_CHARG_PROMPT, (char *)BPrompt, PAGE10_B_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE10_B_CHARG_POWER,(char *)tempbuf, PAGE10_B_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);;											
	XRD_Write_Addr(PAGE10_B_CHARG_MONEY,(char *)tempbuf, PAGE10_B_CHARG_MONEY_LEN);	
	XRD_Load_Page(PAGE10);	
	
}


/************************************************************************************************************
** �� �� �� : DispAChgBChgInfo
** �������� : ��ʾA�����B����н���
** ��    �� :  ASumEnergy   A��ǰ����           0.01kwh
**						 ASumMoney    A��ǰ����           0.01Ԫ
**             ACurrent     A����               0.001 ��3λС��
**             AVoltage     A��ѹ               0.1  V
**             ASumTime     A���ʱ��           S  ������
**             BSumEnergy   B��ǰ����           0.01kwh
**						 BSumMoney    B��ǰ����           0.01Ԫ
**             BCurrent     B����               0.001 ��3λС��
**             BVoltage     B��ѹ               0.1  V
**             BSumTime     B���ʱ��           S  ������  
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAChgBChgInfo( uint32_t ASumEnergy, uint32_t ASumMoney, uint32_t ACurrentA, uint32_t ACurrentB, uint32_t ACurrentC, uint32_t AVoltageA, uint32_t AVoltageB, uint32_t AVoltageC,  uint32_t ASumTime, \
											 uint32_t BSumEnergy, uint32_t BSumMoney, uint32_t BCurrentA, uint32_t BCurrentB, uint32_t BCurrentC, uint32_t BVoltageA, uint32_t BVoltageB, uint32_t BVoltageC,  uint32_t BSumTime )
{
	char tempbuf[56]={0};
	
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE11_A_POWER, (char *)tempbuf, PAGE11_A_POWER_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);
	XRD_Write_Addr(PAGE11_A_COST, (char *)tempbuf, PAGE11_A_COST_LEN);//A��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", ACurrentA / 1000, (ACurrentA % 1000)/10);
	XRD_Write_Addr(PAGE2_A_I_A, (char *)tempbuf, PAGE2_A_I_LEN);//A����A
	sprintf((char *)tempbuf, "%d.%02d", ACurrentB / 1000, (ACurrentB % 1000)/10);
	XRD_Write_Addr(PAGE2_A_I_B, (char *)tempbuf, PAGE2_A_I_LEN);//A����B
	sprintf((char *)tempbuf, "%d.%02d", ACurrentC / 1000, (ACurrentC % 1000)/10);
	XRD_Write_Addr(PAGE2_A_I_C, (char *)tempbuf, PAGE2_A_I_LEN);//A����C	
	sprintf((char *)tempbuf, "%d.%d", AVoltageA / 10, AVoltageA % 10);
	XRD_Write_Addr(PAGE2_A_VOLTAGE, (char *)tempbuf, PAGE2_A_VOLTAGE_LEN);//A��ѹA
	sprintf((char *)tempbuf, "%d.%d", AVoltageB / 10, AVoltageB % 10);
	XRD_Write_Addr(PAGE2_A_VOLTAGE_B, (char *)tempbuf, PAGE2_A_VOLTAGE_LEN);//A��ѹB
	sprintf((char *)tempbuf, "%d.%d", AVoltageC / 10, AVoltageC % 10);
	XRD_Write_Addr(PAGE2_A_VOLTAGE_C, (char *)tempbuf, PAGE2_A_VOLTAGE_LEN);//A��ѹC	
	sprintf((char *)tempbuf, "%02d:%02d:%02d", ASumTime / 3600, (ASumTime % 3600)/60, ((ASumTime % 3600)%60) );
	XRD_Write_Addr(PAGE11_A_CH_TIME, (char *)tempbuf, PAGE11_A_CH_TIME_LEN);//A���ʱ��
	
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE11_B_POWER, (char *)tempbuf, PAGE11_B_POWER_LEN);//B��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);
	XRD_Write_Addr(PAGE11_B_COST, (char *)tempbuf, PAGE11_B_COST_LEN);//B��ǰ����
	sprintf((char *)tempbuf, "%d.%02d", BCurrentA / 1000, (BCurrentA % 1000)/10);
	XRD_Write_Addr(PAGE7_B_I, (char *)tempbuf, PAGE7_B_I_LEN);//B����A
	sprintf((char *)tempbuf, "%d.%02d", BCurrentB / 1000, (BCurrentB % 1000)/10);
	XRD_Write_Addr(PAGE7_B_I_B, (char *)tempbuf, PAGE7_B_I_LEN);//B����B
	sprintf((char *)tempbuf, "%d.%02d", BCurrentC / 1000, (BCurrentC % 1000)/10);
	XRD_Write_Addr(PAGE7_B_I_C, (char *)tempbuf, PAGE7_B_I_LEN);//B����C
	sprintf((char *)tempbuf, "%d.%d", BVoltageA / 10, BVoltageA % 10);
	XRD_Write_Addr(PAGE7_B_VOLTAGE, (char *)tempbuf, PAGE7_B_VOLTAGE_LEN);//B��ѹA
	sprintf((char *)tempbuf, "%d.%d", BVoltageB / 10, BVoltageB % 10);
	XRD_Write_Addr(PAGE7_B_VOLTAGE_B, (char *)tempbuf, PAGE7_B_VOLTAGE_LEN);//B��ѹB
	sprintf((char *)tempbuf, "%d.%d", BVoltageC / 10,BVoltageC % 10);
	XRD_Write_Addr(PAGE7_B_VOLTAGE_C, (char *)tempbuf, PAGE7_B_VOLTAGE_LEN);//B��ѹC
	sprintf((char *)tempbuf, "%02d:%02d:%02d", BSumTime / 3600, (BSumTime % 3600)/60, ((BSumTime % 3600)%60) );
	XRD_Write_Addr(PAGE11_B_CH_TIME, (char *)tempbuf, PAGE11_B_CH_TIME_LEN);//B���ʱ��
	
	XRD_Load_Page(PAGE11);
}




/************************************************************************************************************
** �� �� �� : DispAStopBStopInfo
** �������� : ��ʾAֹͣBֹͣ����
** ��    �� :  APrompt      A��ʾ�ַ���ָ��
**						 AStpReason   Aֹͣԭ���ַ�ָ��
**             ASumEnergy   A������            0.01kwh
**             ASumMoney    A�����            0.01Ԫ
**             BPrompt      B��ʾ�ַ���ָ��
**						 BStpReason   Bֹͣԭ���ַ�ָ��
**             BSumEnergy   B������            0.01kwh
**             BSumMoney    B�����            0.01Ԫ
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispAStopBStopInfo( uint8_t* APrompt, uint8_t* AStpReason, uint32_t ASumEnergy, uint32_t ASumMoney, \
												 uint8_t* BPrompt, uint8_t* BStpReason, uint32_t BSumEnergy, uint32_t BSumMoney )
{	
	char tempbuf[56]={0};
	
	XRD_Write_Addr(PAGE12_A_STOP_REASON, (char *)AStpReason, PAGE12_A_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE12_A_CHARG_PROMPT, (char *)APrompt, PAGE12_A_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", ASumEnergy / 100, ASumEnergy % 100);
	XRD_Write_Addr(PAGE12_A_CHARG_POWER,(char *)tempbuf, PAGE12_A_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", ASumMoney / 100, ASumMoney % 100);;											
	XRD_Write_Addr(PAGE12_A_CHARG_MONEY,(char *)tempbuf, PAGE12_A_CHARG_MONEY_LEN);	
		
	XRD_Write_Addr(PAGE12_B_STOP_REASON, (char *)BStpReason, PAGE12_B_STOP_REASON_LEN);	
	XRD_Write_Addr(PAGE12_B_CHARG_PROMPT, (char *)BPrompt, PAGE12_B_CHARG_PROMPT_LEN);
	sprintf((char *)tempbuf, "%d.%02d", BSumEnergy / 100, BSumEnergy % 100);
	XRD_Write_Addr(PAGE12_B_CHARG_POWER,(char *)tempbuf, PAGE12_B_CHARG_POWER_LEN);											
	sprintf((char *)tempbuf, "%d.%02d", BSumMoney / 100, BSumMoney % 100);;											
	XRD_Write_Addr(PAGE12_B_CHARG_MONEY,(char *)tempbuf, PAGE12_B_CHARG_MONEY_LEN);	
	XRD_Load_Page(PAGE12);	
}





/************************************************************************************************************
** �� �� �� : DispRecordInfo
** �������� : ��ʾ����¼�ĵڼ���
** ��    �� :  StartType    		������ʽ
**						 StartCardID   		����/ID
**             StartDateTime 	  ��ʼʱ��
**             SumTime    			���ʱ��
**             SumEnergy        ������
**						 SumMoney         ���ѽ��
**             StopCause        ֹͣԭ��
**             Line             �ڼ���
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispRecordInfo(uint8_t StartType,uint32_t StartCardID,uint8_t* StartDateTime,uint32_t SumTime, uint32_t SumEnergy,uint32_t SumMoney, uint8_t StopCause, uint8_t Line )
{
	uint8_t tempbuf[56]={0};
	if(StartType == 1){
		sprintf((char *)tempbuf, "%05d", StartCardID);
	}else {
		u32tobcdnomstr1(StartCardID,tempbuf, 12);
		sprintf((char *)tempbuf, "%d%d%d%d%d%d%d%d%d%d", tempbuf[2], tempbuf[3], tempbuf[4], tempbuf[5], tempbuf[6], tempbuf[7], tempbuf[8], tempbuf[9], tempbuf[10], tempbuf[11]);
	}
	XRD_Write_Addr(PAGE28_BASE_RECORD_ID+(PAGE28_STEP_LENGTH * Line),(char *)tempbuf, PAGE28_BASE_RECORD_ID_LEN);			
	sprintf((char *)tempbuf, "%02d/%02d/%02d %02d:%02d:%02d",StartDateTime[0],StartDateTime[1], StartDateTime[2], StartDateTime[3], StartDateTime[4], StartDateTime[5]);			
	XRD_Write_Addr(PAGE28_BASE_RECORD_STARTTIME+(PAGE28_STEP_LENGTH * Line),(char *)tempbuf, PAGE28_BASE_RECORD_STARTTIME_LEN);	
	sprintf((char *)tempbuf, "%02d:%02d:%02d",  SumTime / 3600, ( SumTime  % 3600)/60, (( SumTime  % 3600)%60) );						
	XRD_Write_Addr(PAGE28_BASE_RECORD_CH_TIME+(PAGE28_STEP_LENGTH * Line),(char *)tempbuf, PAGE28_BASE_RECORD_CH_TIME_LEN);		
	sprintf((char *)tempbuf, "%d.%02d",  SumEnergy / 100,  SumEnergy  % 100);
	XRD_Write_Addr(PAGE28_BASE_RECORD_CH_POWER+(PAGE28_STEP_LENGTH * Line),(char *)tempbuf, PAGE28_BASE_RECORD_CH_POWER_LEN);		
	sprintf((char *)tempbuf, "%d.%02d",  SumMoney / 100,  SumMoney  % 100);
	XRD_Write_Addr(PAGE28_BASE_RECORD_CH_MONEY+(PAGE28_STEP_LENGTH * Line),(char *)tempbuf, PAGE28_BASE_RECORD_CH_MONEY_LEN);			
	sprintf((char *)tempbuf, "%02X",  StopCause);
	XRD_Write_Addr(PAGE28_BASE_RECORD_ERR_CODE+(PAGE28_STEP_LENGTH * Line),(char *)tempbuf, PAGE28_BASE_RECORD_ERR_CODE_LEN);	
}

/************************************************************************************************************
** �� �� �� : DispRecordNullInfo
** �������� : ���û�г���¼
** ��    �� : i   	�ڼ���
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispRecordNullInfo(uint8_t i )
{
	XRD_Write_Addr(PAGE28_BASE_RECORD_ID+(PAGE28_STEP_LENGTH * i),(char *)"------------", PAGE28_BASE_RECORD_ID_LEN);	
	XRD_Write_Addr(PAGE28_BASE_RECORD_STARTTIME+(PAGE28_STEP_LENGTH * i),(char *)"------------", PAGE28_BASE_RECORD_STARTTIME_LEN);
	XRD_Write_Addr(PAGE28_BASE_RECORD_CH_TIME+(PAGE28_STEP_LENGTH * i),(char *)"-------", PAGE28_BASE_RECORD_CH_TIME_LEN);
	XRD_Write_Addr(PAGE28_BASE_RECORD_CH_POWER+(PAGE28_STEP_LENGTH * i),(char *)"----", PAGE28_BASE_RECORD_CH_POWER_LEN);
	XRD_Write_Addr(PAGE28_BASE_RECORD_CH_MONEY+(PAGE28_STEP_LENGTH * i),(char *)"----", PAGE28_BASE_RECORD_CH_MONEY_LEN);	
	XRD_Write_Addr(PAGE28_BASE_RECORD_ERR_CODE+(PAGE28_STEP_LENGTH * i),(char *)"--", PAGE28_BASE_RECORD_ERR_CODE_LEN);

}



/************************************************************************************************************
** �� �� �� : DispRecordMoreInfo
** �������� : ��ʾ����¼����
** ��    �� :  ChgPort           �ĸ�ǹ
**             StartType    		������ʽ
**						 StartCardID   		����/ID
**             StartDateTime 	  ��ʼʱ��
**             StopDateTime 	  ֹͣʱ��
**             SumTime    			���ʱ��
**             SumEnergy        ������StartMoney
**             StartMoney       ��ʼ���
**						 SumMoney         ���ѽ��
**             StopCause        ֹͣԭ��
**             StartSOC         ��ʼSOC
**             StopSOC          ֹͣSOC
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispRecordMoreInfo(uint8_t ChgPort,uint8_t StartType,uint32_t StartCardID,uint8_t* StartDateTime, uint8_t* StopDateTime,uint32_t SumTime, uint32_t SumEnergy,uint32_t StartMoney, uint32_t SumMoney,uint8_t StopCause,uint8_t StartSoc, uint8_t StopSoc )
{
	uint8_t tempbuf[56]={0};
	if(StartType == 1){
		sprintf((char *)tempbuf, "%05d", StartCardID);
	}else {
		u32tobcdnomstr1(StartCardID, tempbuf, 12);
		sprintf((char *)tempbuf, "%d%d%d%d%d%d%d%d%d%d", tempbuf[2], tempbuf[3], tempbuf[4], tempbuf[5], tempbuf[6], tempbuf[7], tempbuf[8], tempbuf[9], tempbuf[10], tempbuf[11]);
	}
	XRD_Write_Addr(PAGE29_CARDID_ADDR,(char *)tempbuf, PAGE29_CARDID_ADDR_LEN);
	if(StartType == STARTCHARGETYPE_OFFLINECARD){
		sprintf((char *)tempbuf, "���߿�");
	}else if(StartType == STARTCHARGETYPE_ONLINECARD){
		sprintf((char *)tempbuf, "���߿�");
	}else if(StartType == STARTCHARGETYPE_PLATFORM){
		sprintf((char *)tempbuf, "ƽ̨");
	}
	XRD_Write_Addr(PAGE29_STARTTYPE_ADDR,(char *)tempbuf, PAGE29_STARTTYPE_ADDR_LEN);
	sprintf((char *)tempbuf, "%d.%02d", SumMoney / 100, SumMoney  % 100);
	XRD_Write_Addr(PAGE29_SUMMONEY_ADDR,(char *)tempbuf, PAGE29_SUMMONEY_ADDR_LEN);	
	sprintf((char *)tempbuf, "---" );
	XRD_Write_Addr(PAGE29_STARTSOC_ADDR,(char *)tempbuf, PAGE29_STARTSOC_ADDR_LEN);	//	STARTSOC
	sprintf((char *)tempbuf, "---" );
	XRD_Write_Addr(PAGE29_STOPSOC_ADDR,(char *)tempbuf, PAGE29_STOPSOC_ADDR_LEN);	//	STOPSOC	
	sprintf((char *)tempbuf, "%02d/%02d/%02d %02d:%02d:%02d", StartDateTime[0],StartDateTime[1],StartDateTime[2],StartDateTime[3],StartDateTime[4],StartDateTime[5]);			
	XRD_Write_Addr(PAGE29_STARTTIME_ADDR,(char *)tempbuf, PAGE29_STARTTIME_ADDR_LEN);
	sprintf((char *)tempbuf, "%02d:%02d:%02d", SumTime / 3600, ( SumTime  % 3600)/60, (( SumTime  % 3600)%60) );
	XRD_Write_Addr(PAGE29_SUMTIME_ADDR,(char *)tempbuf, PAGE29_SUMTIME_ADDR_LEN);
	if(ChgPort == 0){
		sprintf((char *)tempbuf, "A ǹ");
	}else{
		sprintf((char *)tempbuf, "B ǹ");
	}	
	XRD_Write_Addr(PAGE29_CHGPORT_ADDR,(char *)tempbuf, PAGE29_CHGPORT_ADDR_LEN);		
	sprintf((char *)tempbuf, "%d.%02d", SumEnergy / 100, SumEnergy  % 100);
	XRD_Write_Addr(PAGE29_SUMENERGY_ADDR,(char *)tempbuf, PAGE29_SUMENERGY_ADDR_LEN);	
	sprintf((char *)tempbuf, "%02X", StopCause);
	XRD_Write_Addr(PAGE29_STOPCAUSE_ADDR,(char *)tempbuf, PAGE29_STOPCAUSE_ADDR_LEN);	
												
	sprintf((char *)tempbuf, "%02d/%02d/%02d %02d:%02d:%02d", StopDateTime[0],StopDateTime[1],StopDateTime[2],StopDateTime[3],StopDateTime[4],StopDateTime[5]);			
	XRD_Write_Addr(PAGE29_STOPTIME_ADDR,(char *)tempbuf, PAGE29_STOPTIME_ADDR_LEN);	
	sprintf((char *)tempbuf, "%d.%02d", StartMoney / 100, StartMoney  % 100);
	XRD_Write_Addr(PAGE29_STARTMONEY_ADDR,(char *)tempbuf, PAGE29_STARTMONEY_ADDR_LEN);	
	XRD_Load_Page(PAGE29);							
	
	
}



/************************************************************************************************************
** �� �� �� : DispUpdataInfo
** �������� : ��ʾ�����������
** ��    �� :  Present    		�����ٷֱ�
** ��    �� : ��
** ��    �� :	��
*************************************************************************************************************
*/
void DispUpdataInfo( uint8_t Present, uint8_t* Tip)
{
	uint8_t tempbuf[56]={0};
	sprintf((char *)tempbuf, "%02d%%",Present );
	XRD_Write_Addr(PAGE30_UPDATA_PERSENT_ADDR,(char *)tempbuf, PAGE30_UPDATA_PERSENT_ADDR_LEN);
	XRD_Write_Addr(PAGE30_UPDATA_TIP_ADDR,(char *)Tip, PAGE30_UPDATA_TIP_ADDR_LEN);
	XRD_Load_Page(PAGE30);					
}











