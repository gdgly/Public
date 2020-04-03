#ifndef 	__LCM_H
#define	__LCM_H

#include "stm32f4xx.h"
#include "lcd_fonts.h"

/*------------------------------  引脚配置宏 ----------------------------------*/	

#define 		LCD_RST_PIN      GPIO_Pin_0				
#define 		LCD_RST_PORT     GPIOB                  
#define 		LCD_RST_CLK      RCC_AHB1Periph_GPIOB	

#define 		LCD_RS_PIN       GPIO_Pin_6
#define 		LCD_RS_PORT      GPIOA                  
#define 		LCD_RS_CLK       RCC_AHB1Periph_GPIOA	


#define 		LCD_DC_PIN       GPIO_Pin_4
#define 		LCD_DC_PORT      GPIOA                  
#define 		LCD_DC_CLK       RCC_AHB1Periph_GPIOA	

#define 		LCD_SCK_PIN      GPIO_Pin_5
#define 		LCD_SCK_PORT     GPIOA                  
#define 		LCD_SCK_CLK      RCC_AHB1Periph_GPIOA

#define 		LCD_SDA_PIN      GPIO_Pin_3
#define 		LCD_SDA_PORT     GPIOA                  
#define 		LCD_SDA_CLK      RCC_AHB1Periph_GPIOA	

/*------------------------------- 控制宏 ------------------------------------*/

#define 		LCD_RST_H     GPIO_SetBits(LCD_RST_PORT,LCD_RST_PIN)		
#define 		LCD_RST_L     GPIO_ResetBits(LCD_RST_PORT,LCD_RST_PIN)


#define 		LCD_RS_H     GPIO_SetBits(LCD_RS_PORT,LCD_RS_PIN)		
#define 		LCD_RS_L     GPIO_ResetBits(LCD_RS_PORT,LCD_RS_PIN)
				
#define 		LCD_DC_H      GPIO_SetBits(LCD_DC_PORT,LCD_DC_PIN)
#define 		LCD_DC_L      GPIO_ResetBits(LCD_DC_PORT,LCD_DC_PIN)
				
#define 		LCD_SCK_H     GPIO_SetBits(LCD_SCK_PORT,LCD_SCK_PIN)
#define 		LCD_SCK_L     GPIO_ResetBits(LCD_SCK_PORT,LCD_SCK_PIN)
				
#define 		LCD_SDA_H     GPIO_SetBits(LCD_SDA_PORT,LCD_SDA_PIN)
#define 		LCD_SDA_L     GPIO_ResetBits(LCD_SDA_PORT,LCD_SDA_PIN)

/*------------------------------- 参数宏 ------------------------------------*/

#define 	LCD_Contrast	38		// 微调屏幕对比度，可设置范围0x00～0x3f，用户需要根据自己的屏幕进行微调，以达到最佳的显示效果
#define  	Fill_Zero  		0			// 填充0
#define  	Fill_Space 		1			// 填充空格
	
#define  	Cursor_Reverse  	1		//	反白光标
#define  	Cursor_Underline  2		// 下划线光标
		
#define  	ON   1 						// 开启
#define  	OFF  0						// 关闭

/*------------------------------- 函数列表 ------------------------------------*/

void LCD_Init(void);		//	初始化LCD屏幕，进行升压、对比度的配置
void LCD_Clear(void);	//	清除屏幕
void LCD_ClearLine(uint8_t line); // 清除行

void LCD_SetFont(pFONT *fonts);		// 设置要显示的字体
void LCD_SetReverse(u8 mode);			//	开启反白，接下来显示的内容全部以反白的模式显示
void LCD_SetCursor(u8 n,u8 mode);	// 设置光标要显示的位置以及光标的模式
void LCD_ShowNumMode(u8 mode);		// 设置数字高位为0时的显示方式

void LCD_DisplayString( u8 x, u8 y, u8 *p);					// 在指定坐标处显示ASCII字符串
void LCD_DisplayNumber(u8 x, u8 y, u32 number, u8 len);	// 在指定坐标处显示数字变量，取值范围 0~4294967295
void LCD_DisplayText( u8 x, u8 y, u8 *pText);				// 在指定坐标处显示文本，可显示汉字或ASCII码
void LCD_DrawImage(u8 x,u8 y,u8 width,u8 height,const u8 *pImage); // 在指定坐标处显示图片
void LCDClearArea(u8 x,u8 y,u8 width,u8 height);//清除指定位置屏幕

void LCD_GPIO_Config(void);								// 初始化LCD的GPIO口,推挽输出
void LCD_Address(u8 x,u8 y);								// 设置需要显示数据的坐标
void LCD_DisplayChar(u8 x, u8 y,u8 ASCII);			//	在指定坐标处显示单个ASCII码
void LCD_DisplayChinese(u8 x, u8 y, u8 *pText);	 	// 在指定坐标处显示单个汉字		
void WriteCom_7565(u8 com);			//写命令
void WriteData_7565(u8 dat);			//写数据

#endif 
