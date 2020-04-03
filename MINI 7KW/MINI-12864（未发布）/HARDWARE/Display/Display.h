#ifndef __DISPLAY_H
#define __DISPLAY_H	 
#include "sys.h"

void DisStartDev(void);
void DispUpdataInfo(uint8_t pre, uint8_t Tip);
void DisQrcode(uint8_t *buf);

void DisHeadInfo(uint8_t online);

void DispNoPileUsed(void);
void DispInsertGunInfo(void);
void DispOperationInfo(void);
void DispUnLockFailureInfo(uint8_t code);
void DispCardLockInfo(uint8_t *buf);

void DispDeviceFault(uint8_t* buf);

void DispRemainMoneyInfo(uint32_t Money);
void DispVerifyFailure(uint8_t code);
void DispInVerify(void);

void DispStartSucessInfo(void);
void DispStartFailureInfo(void);
void DispStartChgInfo(void);

void DispAIdleInfo(uint8_t gun, uint8_t* APileNum);
void DispAChgInfo(uint32_t SumEnergy,uint32_t SumMoney, uint32_t Current,uint32_t Voltage,uint32_t SumTime);
void DispAStopInfo( uint8_t StopReason, uint32_t SumEnergy, uint32_t SumMoney );


void DispAccount1(uint32_t SumEnergy, uint32_t SumMoney, uint32_t RemainMoney, uint32_t SumTime, uint8_t* Date);
void DispAccount2(uint32_t SumEnergy, uint32_t SumMoney, uint32_t RemainMoney, uint32_t SumTime, uint8_t* Date);
void TestDis(void); 				    
#endif
