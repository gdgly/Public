#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  



typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.   
void iap_load_app(u32 appxaddr);			//��ת��APP����ִ��

#endif







































