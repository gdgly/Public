#ifndef __FONTS_H
#define __FONTS_H

#include <stdint.h>

// ������ؽṹ����
typedef struct _pFont
{    
  const uint8_t *table;  //��ģ�����ַ
  uint16_t Width;  //��ģ���
  uint16_t Height; //��ģ����
  uint16_t Sizes;	 //��ģ����
} pFONT;

extern pFONT Font08; // 0805 ����
extern pFONT Font16; // 1608 ����
extern pFONT Font24; // 2412 ����
extern pFONT Font32; // 3216 ����

extern const uint8_t  Chinese_Table[];    	// ���ּ�����
extern const uint8_t GBK2312_1212_Table[];
extern const uint8_t  GBK2312_1616_Table[];	// �����ֿ�
extern const uint8_t IconOnline8_8[];				//����ͼ��ȡģ
extern const uint8_t IconOnline16_16[]; 		//����ͼ��ȡģ
extern const uint8_t IconGun8_32[]; 				//ǹͼ��ȡģ
#endif 
 
