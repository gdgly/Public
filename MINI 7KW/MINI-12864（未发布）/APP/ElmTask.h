#ifndef _ELMTASK_H
#define _ELMTASK_H
#include "sys.h"

typedef struct
{
	volatile uint8_t  State;					// 状态
	volatile uint32_t Totalelc;   		// 当前总电量  1/100KWH
	volatile uint32_t RawTotalelc;  	//	原始寄存器电量
	volatile uint32_t Vol;			  		// 电压	1/10 v
	volatile uint32_t RawVol;		  	// 原始寄存器电压
	volatile uint32_t Cur;		  			// 电流   1/1000 A
	volatile uint32_t RawCur;		  	//  原始寄存器电流
	volatile uint32_t Kw;		  			// 原始原始功率
	volatile uint32_t RawKw;		  		//  功率
}ELMINFO;








//计量参数寄存器
#define MPREG_SPLI1             0x00 //电流通道1的ADC采样数据
#define MPREG_SPLI2             0x01 //电流通道2的ADC采样数据
#define MPREG_SPLU              0x02 //电压通道的ADC采样数据
#define MPREG_IDC               0x03 //I通道直流均值
#define MPREG_UDC               0x04 //U通道直流均值
#define MPREG_RMSI1             0x06 //电流通道1的有效值
#define MPREG_RMSI2             0x07 //电流通道2的有效值
#define MPREG_RMSU              0x08 //电压通道的有效值
#define MPREG_FREQU             0x09 // 电压频率          
#define MPREG_POWERP1           0x0A //第一通道有功功率
#define MPREG_POWERQ1           0x0B //第一通道无功功率
#define MPREG_POWERS            0x0C //视在功率
#define MPREG_ENERGYP           0x0D //有功能量
#define MPREG_ENERGYQ           0x0E //无功能量
#define MPREG_UDETCNT           0x0F //SAG/Peak工况持续时间计数
#define MPREG_POWERP2           0x10 //第二通道有功功率
#define MPREG_POWERQ2           0x11 //第二通道无功功率
#define MPREG_MAXUWAVE          0x12 //电压半波波形峰值寄存器
#define MPREG_CRCCHECKSUM       0x15 //校表参数校验和寄存器(CRC16)
#define MPREG_BACKUPDATA        0x16 // 通讯数据备份寄存器
#define MPREG_COMCHECKSUM       0x17 //通讯校验和寄存器
#define MPREG_SUMCHECKSUM       0x18 //校表参数校验和寄存器
#define MPREG_EMUSR             0x19 //EMU状态寄存器
#define MPREG_SYSSTA            0x1A //系统状态寄存器
#define MPREG_CHIPID            0x1B //ChipID，默认值为7053B0
#define MPREG_DEVICEID          0x1C //DeviceID，默认值为705321

//校表参数寄存器

#define ECADR_EMUIE               0x30 //中断使能寄存器
#define ECADR_EMUIF               0x31 //EMU中断 标志寄存器
#define ECADR_WPREG               0x32 //写保护寄存器
#define ECADR_SRST								0x33 //软件复位寄存器
#define ECADR_EMUCFG              0x40 //EMU配置寄存器
#define ECADR_FREGCFG             0x41 //时钟/更新频率配置寄存器
#define ECADR_MODULEEN            0x42 //EMU模块使能寄存器
#define ECADR_ANAEN               0x43 //ADC开关寄存器
#define ECADR_IOCFG               0x45 //IO输出配置寄存器
#define ECADR_GP1                 0x50 //通道1的有功功率校正
#define ECADR_GQ1                 0x51 //通道1的无功功率校正
#define ECADR_GS1                 0x52 //通道1的视在功率校正

#define ECADR_GP2                 0x54 //通道2的有功功率校正
#define ECADR_GQ2                 0x55 //通道2的无功功率校正
#define ECADR_GS2                 0x56 //通道2的视在功率校正
#define ECADR_QPHSCAL             0x58 //无功相位补偿
#define ECADR_ADCCON              0x59 //ADC通道增加
#define ECADR_I2GAIN              0x5B //电流通道2增益补偿
#define ECADR_I1OFF               0x5C //电流通道1的偏置校正
#define ECADR_I2OFF               0x5D //电流通道2的偏置校正
#define ECADR_UOFF                0x5E //电压通道的偏置校正
#define ECADR_PQSTART             0x5F //起动功率设置
#define ECADR_HFCONST             0x61 //输出脉冲功率设置
#define ECADR_DEC_SHIFT           0x64 //通道1的相位校正
#define ECADR_P1OFFSETH           0x65 //通道1有功功率设置校正参数高8位和PIOOFFSETL组成的16bit补码
#define ECADR_P2OFFSETH           0x66 //通道2有功功率设置校正参数高8位和PIOOFFSETL组成的16bit补码
#define ECADR_Q1OFFSETH           0x67 //通道1无功功率设置校正参数高8位和PIOOFFSETL组成的16bit补码
#define ECADR_Q2OFFSETH           0x68 //通道2无功功率设置校正参数高8位和PIOOFFSETL组成的16bit补码
#define ECADR_I1RMSOFFSET         0x69 //通道1有效值补偿寄存器，为16bit无符号数
#define ECADR_I2RMSOFFSET         0x6A //通道2有效值补偿寄存器，为16bit无符号数
#define ECADR_URMSOFFSET          0x6B //通道U有效值补偿寄存器，为16bit无符号数
#define ECADR_ZCROSSCURRENT       0x6C //电流过零阈值设置寄存器
#define ECADR_GPHS1               0x6D //通道1的相位校正（PQ方式）               
#define ECADR_GPHS2               0x6E //通道2的相位校正（PQ方式）
#define ECADR_PFCNT               0x6F //快速有功脉冲计数              
#define ECADR_QFCNT               0x70 //快速无功脉冲计数
#define ECADR_ANACON              0x72 //模拟控制寄存器
#define ECADR_SUMCHECKL           0x73 //校验和低16位，由用户写入，使能比较功能后，芯片比较给出标志
#define ECADR_SUMCHECKH           0x74 //检验和高8位，由用户写入，使能比较功能后，芯片比较给出标志
#define ECADR_MODECFG             0x75 //模式配置寄存器
#define ECADR_P1OFFSETL           0x76 //通道1有功功率偏置校正参数低8位，和P1OFFSETH组成16bit 补码
#define ECADR_P2OFFSETL           0x77 //通道2有功功率偏置校正参数低8位，和P2OFFSETH组成16bit 补码 
#define ECADR_Q1OFFSETL           0x78 //通道1无功功率偏置校正参数低8位，和Q1OFFSETH组成16bit 补码
#define ECADR_Q2OFFSETL           0x79 //通道2无功功率偏置校正参数低8位，和Q2OFFSETH组成16bit 补码
#define ECADR_UPEAKLV1            0x7A //UPEAK阈值寄存器，16位无符号数，与ADC绝对值的高位对齐
#define ECADR_USAGLV1             0x7B //USAG阈值寄存器，16位无符号数，与ADC绝对值的高位对齐
#define ECADR_UCYLEN              0x7C //PEAK SAG检测周期设置寄存器，16bit

#define HEAD                      0x6A  //帧头




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




