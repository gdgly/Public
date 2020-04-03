#ifndef __FONTS_H
#define __FONTS_H

#include <stdint.h>

// 字体相关结构定义
typedef struct _pFont
{    
  const uint8_t *table;  //字模数组地址
  uint16_t Width;  //字模宽度
  uint16_t Height; //字模长度
  uint16_t Sizes;	 //字模个数
} pFONT;

extern pFONT Font08; // 0805 字体
extern pFONT Font16; // 1608 字体
extern pFONT Font24; // 2412 字体
extern pFONT Font32; // 3216 字体

extern const uint8_t  Chinese_Table[];    	// 汉字检索表
extern const uint8_t GBK2312_1212_Table[];
extern const uint8_t  GBK2312_1616_Table[];	// 汉字字库
extern const uint8_t IconOnline8_8[];				//在线图标取模
extern const uint8_t IconOnline16_16[]; 		//在线图标取模
extern const uint8_t IconGun8_32[]; 				//枪图标取模
#endif 
 
