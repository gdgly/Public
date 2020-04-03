#include "sys.h"

#include "Cfg.h"
#include "string.h"
#include "stdio.h"
#include "MainTask.h"
#include "rtc.h"
#include "timer.h"
#include "QR_Encode.h"
#include "includes.h"
#include "Para.h"
#include "lcd_12864.h"
#include "lcd_fonts.h"

static uint8_t ClearFlag;  //清屏标志，





/************************************************************************************************************
** 函 数 名 : DisStartDev
** 功能描述 : 显示设备启动中
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DisStartDev(void)
{
	uint8_t tempbuf[32];
	Rtc rtc;
	if(ClearFlag != 1){
		ClearFlag = 1;
		LCD_Clear();
	}
	
	LCD_SetFont(&Font16);	// 设置字体
	LCD_DisplayText(16,2,(u8 *)"Device starting");
	LCD_SetFont(&Font08);	
	sprintf((char *)tempbuf, "V%d.%d.%d", _VERSION_MAJOR,_VERSION_MINOR, _VERSION_PATCH);
	LCD_DisplayText(48,5,tempbuf);
	RtcRead(&rtc);
	sprintf((char *)tempbuf, "%d/%02d/%02d %02d:%02d:%02d", rtc.year, rtc.month,rtc.day ,rtc.hour, rtc.min,rtc.sec);
	LCD_DisplayText(8, 7,(u8 *)tempbuf);

}

/************************************************************************************************************
** 函 数 名 : DispAIdleInfo
** 功能描述 : 显示待机界面
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAIdleInfo(uint8_t gun, uint8_t* APileNum)
{
	if(ClearFlag != 2){
		ClearFlag = 2;
		LCD_Clear();
		printf("待机\r\n");
	}
	if(gun){	
		LCD_DrawImage(8,4,8,32,IconGun8_32);
	}else{
		LCDClearArea(8,4,8,32);		
	}
		
	LCD_SetFont(&Font16);	// 设置字体
	LCD_DisplayText(24, 6,APileNum);
	LcdShowQrCode(50, 2);  //显示二维码
	
}



/************************************************************************************************************
** 函 数 名 : DispAChgInfo1
** 功能描述 : 显示充电界面
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAChgInfo(uint32_t SumEnergy,uint32_t SumMoney, uint32_t Current,uint32_t Voltage,uint32_t SumTime)
{
	char tempbuf[56]={0};
	uint8_t page;
	static uint32_t DisTimer;

	LCD_SetFont(&Font16);	// 设置字体	

	if((ClearFlag !=3) && (ClearFlag !=4)){
		page = 1;
		printf("充电时长：%d\r\n", SumTime);
	}else{
		if(ClearFlag == 3){
			if(TimerRead() - DisTimer > T1S *5)
				page = 0;
		}else{
			if(TimerRead() - DisTimer > T1S *5)
				page = 1;
		}
	}

	if(page == 1){
		if(ClearFlag != 3){
			ClearFlag = 3;
			LCD_Clear();
			DisTimer = TimerRead();
		}
		sprintf((char *)tempbuf, "%02d:%02d:%02d", SumTime / 3600, (SumTime % 3600)/60, ((SumTime % 3600)%60) );
		LCD_DisplayText(40, 2,(u8 *)tempbuf);
		sprintf((char *)tempbuf, "电量:%d.%02d", SumEnergy / 100, SumEnergy % 100);
		LCD_DisplayText(1, 4,(u8 *)tempbuf);
		sprintf((char *)tempbuf, "金额:%d.%02d", SumMoney / 100, SumMoney % 100);
		LCD_DisplayText(1, 6,(u8 *)tempbuf);
	}else{
		if(ClearFlag != 4){
			ClearFlag = 4;
			LCD_Clear();
			DisTimer = TimerRead();
		}
		sprintf((char *)tempbuf, "%02d:%02d:%02d", SumTime / 3600, (SumTime % 3600)/60, ((SumTime % 3600)%60) );
		LCD_DisplayText(40, 2,(u8 *)tempbuf);
		sprintf((char *)tempbuf, "电流:%d.%02d", Current / 1000,(Current % 1000)/10);
		LCD_DisplayText(1, 4,(u8 *)tempbuf);
		sprintf((char *)tempbuf, "电压:%d.%02d", Voltage / 10, Voltage % 10);
		LCD_DisplayText(1, 6,(u8 *)tempbuf);
	}
}








/************************************************************************************************************
** 函 数 名 : DispAStopInfo
** 功能描述 : 显示充电停止界面
** 输    入 : 无
** 输    出 : 无
** 返    回 :	无
*************************************************************************************************************
*/
void DispAStopInfo( uint8_t StopReason, uint32_t SumEnergy, uint32_t SumMoney )
{
	char tempbuf[56]={0};
	LCD_SetFont(&Font16);	// 设置字体
	if(ClearFlag != 5){
		ClearFlag = 5;
		LCD_Clear();
		printf("停止\r\n");
	}
	sprintf((char *)tempbuf, "****%s****",(uint8_t *)stopcausetab[StopReason]);
	LCD_DisplayText(0, 6,(u8 *)tempbuf);
	sprintf((char *)tempbuf, "电量:%d.%02d", SumEnergy / 100, SumEnergy % 100);
	LCD_DisplayText(1, 2,(u8 *)tempbuf);
	sprintf((char *)tempbuf, "金额:%d.%02d", SumMoney / 100, SumMoney % 100);
	LCD_DisplayText(1, 4,(u8 *)tempbuf);
}



void DispRemainMoneyInfo(uint32_t Money)
{
	char tempbuf[56]={0};
	LCD_SetFont(&Font16);	// 设置字体
	if(ClearFlag != 6){
		ClearFlag = 6;
		LCD_Clear();
		printf("余额\r\n");
	}
	sprintf((char *)tempbuf, "余额:%d.%02d元", Money / 100, Money % 100);
	LCD_DisplayText(32, 6,(u8 *)tempbuf);
}

void DispAccount1(uint32_t SumEnergy, uint32_t SumMoney, uint32_t RemainMoney, uint32_t SumTime, uint8_t* Date)
{
	char tempbuf[56]={0};
	LCD_SetFont(&Font16);	// 设置字体
	if(ClearFlag != 7){
		ClearFlag = 7;
		LCD_Clear();
		printf("结算成功\r\n");
	}
	LCD_DisplayText(16, 1,(u8 *)"***结算成功***");
	sprintf((char *)tempbuf, "电量:%d.%02d", SumEnergy / 100, SumEnergy % 100);
	LCD_DisplayText(1, 3,(u8 *)tempbuf);
	sprintf((char *)tempbuf, "充电时长:%dMin", SumTime / 60);
	LCD_DisplayText(1, 5,(u8 *)tempbuf);
	LCD_SetFont(&Font08);	// 设置字体
	sprintf((char *)tempbuf, "20%02d/%02d/%02d %02d:%02d:%02d", Date[0], Date[1], Date[2], Date[3], Date[4],Date[5]);
	LCD_DisplayText(15, 7,(u8 *)tempbuf);
}
void DispAccount2(uint32_t SumEnergy, uint32_t SumMoney, uint32_t RemainMoney, uint32_t SumTime, uint8_t* Date)
{
	char tempbuf[56]={0};
	LCD_SetFont(&Font16);	// 设置字体
	if(ClearFlag != 8){
		ClearFlag = 8;
		LCD_Clear();
	}
	LCD_DisplayText(16, 1,(u8 *)"***结算成功***");
	sprintf((char *)tempbuf, "余额:%d.%02d", RemainMoney / 100, RemainMoney % 100);
	LCD_DisplayText(1, 3,(u8 *)tempbuf);
	sprintf((char *)tempbuf, "使用金额:%d.%02d", SumMoney / 100, SumMoney % 100);
	LCD_DisplayText(1, 5,(u8 *)tempbuf);
	LCD_SetFont(&Font08);	// 设置字体
	sprintf((char *)tempbuf, "20%02d/%02d/%02d %02d:%02d:%02d", Date[0], Date[1], Date[2], Date[3], Date[4],Date[5]);
	LCD_DisplayText(15, 7,(u8 *)tempbuf);
}



void DispStartChgInfo(void)
{
	if(ClearFlag != 9){
		ClearFlag = 9;
		LCD_Clear();
		printf("充电启动中\r\n");
	}
	LCD_Clear();	// 清屏
	LCD_SetFont(&Font16);	// 设置字体
	LCD_DisplayText(16, 6,(u8 *)"***充电启动中***");
}




void DispStartSucessInfo(void)
{

	if(ClearFlag != 10){
		ClearFlag = 10;
		printf("启动成功\r\n");
		LCD_Clear();
	}
	LCD_SetFont(&Font16);	// 设置字体
	LCD_DisplayText(20, 6,(u8 *)"***启动成功***");
}

void DispStartFailureInfo(void)
{
	if(ClearFlag != 11){
		ClearFlag = 11;
		LCD_Clear();
		printf("启动失败\r\n");
	}
	LCD_SetFont(&Font16);	// 设置字体
	LCD_DisplayText(20, 6,(u8 *)"***启动失败***");

}





void DispInsertGunInfo(void)
{

	if(ClearFlag != 12){
		ClearFlag = 12;
		LCD_Clear();
		printf("请插枪\r\n");
	}
	LCD_SetFont(&Font16);	// 设置字体
	LCD_DisplayText(24, 6,(u8 *)"***请插枪***");
	LCD_DrawImage(112,4,8,32,IconGun8_32);
}



void DispDeviceFault(uint8_t* pbuf)
{
	uint8_t buf[126];
	if(ClearFlag != 13){
		ClearFlag = 13;
		LCD_Clear();
		printf("故障\r\n");
	}
	LCD_SetFont(&Font16);	// 设置字体
	sprintf((char *)buf,"故障:%s       ",pbuf);
	LCD_DisplayText(32, 3,buf);
	
	ParaGetPileNo(0, buf);
	strcat((char* )buf,"A");
	LCD_DisplayText(24, 6,buf);
}


void DispNoPileUsed(void)
{
	if(ClearFlag != 14){
		ClearFlag = 14;
		LCD_Clear();
		printf("***正在充电中***\r\n");
	}
	LCD_SetFont(&Font16);	// 设置字体
	LCD_DisplayText(16, 6,(u8 *)"***正在充电中***");
}



void DispInVerify(void)
{
	if(ClearFlag != 15){
		ClearFlag = 15;
		LCD_Clear();
		printf("***正在验证中***\r\n");
	}
	LCD_SetFont(&Font16);	// 设置字体
	LCD_DisplayText(16, 6,(u8 *)"***正在验证中***");
}


void DispVerifyFailure(uint8_t code)
{
	char tempbuf[16]={0};
	if(ClearFlag != 16){
		ClearFlag = 16;
		LCD_Clear();
		printf("***验证失败***\r\n");
	}
	LCD_SetFont(&Font16);	// 设置字体
	LCD_DisplayText(20, 4,(u8 *)"***验证失败***");
	sprintf((char *)tempbuf, "%02d", code);
	LCD_DisplayText(32, 6,(u8 *)tempbuf);
}     



void DispOperationInfo()
{
	if(ClearFlag != 17){
		ClearFlag = 17;
		LCD_Clear();
	}
	LCD_SetFont(&Font16);	// 设置字体
	LCD_DisplayText(16, 6,(u8 *)"***正在操作中***");

}


void DispUnLockFailureInfo(uint8_t code)
{
	if(ClearFlag != 18){
		ClearFlag = 18;
		LCD_Clear();
		if(code != UNLOCKCARDFAILCODE_WRITECARD)
			printf("***未找到记录***\r\n");
	}
	LCD_SetFont(&Font16);	// 设置字体
	if(code == UNLOCKCARDFAILCODE_WRITECARD)
		LCD_DisplayText(20, 6,(u8 *)"***写卡失败***");
	else
		LCD_DisplayText(16, 6,(u8 *)"***未找到记录***");
}


void DispCardLockInfo(uint8_t *buf)
{
	if(ClearFlag != 19){
		ClearFlag = 19;
		LCD_Clear();
		printf("***其他桩锁住***\r\n");
		
	}
	LCD_SetFont(&Font16);	// 设置字体
	LCD_DisplayText(48, 5,buf);
}


void DispUpdataInfo(uint8_t pre, uint8_t Tip)
{
	char tempbuf[16]={0};
	if(ClearFlag != 20){
		ClearFlag = 20;
		LCD_Clear();
	}
	LCD_SetFont(&Font16);	// 设置字体
	if(Tip == 1){
		sprintf((char *)tempbuf, "%02d%%", pre);
		LCD_DisplayText(40, 4, (u8 *)tempbuf);
		LCD_DisplayText(20, 6, (u8 *)"***正在更新***");
	}else if(Tip == 2){
		LCD_DisplayText(20, 6, (u8 *)"***更新成功***");
	}else if(Tip == 3){
		LCD_DisplayText(20, 6, (u8 *)"***更新失败***");
	}
		
}



void DisHeadInfo(uint8_t online)
{
	Rtc rtc;
	uint8_t timebuf[32];
	RtcRead(&rtc);
	sprintf((char *)timebuf,"%02d:%02d",rtc.hour,rtc.min);
	LCD_SetFont(&Font08);
	LCD_DisplayText(4,0,timebuf);
	if(online)
		LCD_DrawImage(120,0,8,8,IconOnline8_8);
	else
		LCDClearArea(120,0,8,8);
		
		
}


void TestDis(void)
{
	u8 data[6];
	
	data[0] = 19;
	data[1] = 7;
	data[2] = 19;
	data[3] = 23;
	data[4] = 11;
	data[5] = 16;
	
	DisStartDev();
	OSTimeDlyHMSM(0,0,2,0);
	
	DispAIdleInfo(1, "1245002001A");
	OSTimeDlyHMSM(0,0,2,0);
	
	DispAIdleInfo(0, "1245002001A");
	OSTimeDlyHMSM(0,0,2,0);
	
	DispAChgInfo(750,1526, 49632,2200,4890);
	OSTimeDlyHMSM(0,0,2,0);
	
	DispAStopInfo(0x0C, 750,1526);
	OSTimeDlyHMSM(0,0,2,0);
	
	DispAccount1(750, 1526, 15260, 43320,data);
	OSTimeDlyHMSM(0,0,2,0);
	DispAccount2(750, 1526, 15260, 43320,data);
	OSTimeDlyHMSM(0,0,2,0);
	
	DispStartSucessInfo();
	OSTimeDlyHMSM(0,0,2,0);
	
	DispStartFailureInfo();
	OSTimeDlyHMSM(0,0,2,0);
	
	DispStartChgInfo();
	OSTimeDlyHMSM(0,0,2,0);
	
	DispInVerify();
	OSTimeDlyHMSM(0,0,2,0);
	
	DispVerifyFailure(0x00);
	OSTimeDlyHMSM(0,0,2,0);
	
	DispRemainMoneyInfo(10000);
	OSTimeDlyHMSM(0,0,2,0);
	
	DispDeviceFault("01 03");
	OSTimeDlyHMSM(0,0,2,0);
	
	DispNoPileUsed();
	OSTimeDlyHMSM(0,0,2,0);
	
	DispInsertGunInfo();
	OSTimeDlyHMSM(0,0,2,0);
	
	DispOperationInfo();
	OSTimeDlyHMSM(0,0,2,0);
	
	DispUnLockFailureInfo(1);
	OSTimeDlyHMSM(0,0,2,0);
	
	DispCardLockInfo("1122");
	OSTimeDlyHMSM(0,0,2,0);






}



