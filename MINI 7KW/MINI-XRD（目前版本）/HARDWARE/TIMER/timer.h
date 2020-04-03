#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"


#define      T10MS     1		// 10ms	
#define      T100MS    10		// 100ms
#define      T1S			 100	// 1s	
#define      T1M			 6000   // 1M
#define      T1H			 360000  // 1H
void TIMInit(u16 arr,u16 psc);
uint32_t TimerRead(void);
#endif
