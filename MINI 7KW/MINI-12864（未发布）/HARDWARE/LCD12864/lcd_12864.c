
#include "lcd_12864.h"   

static pFONT *LCD_Fonts;	//字库结构体指针

// LCD相关参数结构体
struct		
{	u8  Reverse_Mode;			//反白模式
	u8  ShowNum_Mode;			//数字显示模式，用于确定空余位补0还是补空格
	u8	 Cursor_Mode;			//光标模式
	u8  Cursor_Location;		//光标位置
	u8  Cursor_ShowFlag;		//光标显示标志
}LCD;

/*****************************************************************************************
*	函 数 名: LCD_Delay
*	入口参数: a - 延时时间，单位ms
*	返 回 值: 无
*	函数功能: 简单延时函数，实现ms级的延时
*	说    明: 为了移植的简便性且对延时精度要求不高，所以不需要使用定时器做延时
******************************************************************************************/

void LCD_Delay(u16 a)
{
	u16 i;
	while(a--)
	{
		for ( i = 0; i < 5000; i++);
	}
}

/*****************************************************************************************
*	函 数 名: LCD_GPIO_Config
*	入口参数: 无
*	返 回 值: 无
*	函数功能: 初始化LCD的GPIO口,推挽输出
*	说    明: 无
******************************************************************************************/

void LCD_GPIO_Config(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd ( LCD_RST_CLK|LCD_DC_CLK|LCD_SCK_CLK|LCD_SDA_CLK, ENABLE); //开启LCD控制引脚的时钟
													
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  //无上下啦
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 

  GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN;		//初始化RST引脚
	GPIO_Init(LCD_RST_PORT, &GPIO_InitStructure);	
	
	

  GPIO_InitStructure.GPIO_Pin = LCD_RS_PIN;			//初始化DC引脚
	GPIO_Init(LCD_RS_PORT, &GPIO_InitStructure);	

  GPIO_InitStructure.GPIO_Pin = LCD_DC_PIN;			//初始化DC引脚
	GPIO_Init(LCD_DC_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = LCD_SCK_PIN;		//初始化SCK引脚
	GPIO_Init(LCD_SCK_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = LCD_SDA_PIN;		//初始化SDA引脚
	GPIO_Init(LCD_SDA_PORT, &GPIO_InitStructure);	
	
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);		
	GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_3);
}

/*****************************************************************************************
*	函 数 名: WriteCom_7565
*	入口参数: com - 需要写入的控制指令
*	返 回 值: 无
*	函数功能: 用于写入控制字
*	说    明: 无
******************************************************************************************/

void WriteCom_7565(u8 com)	//写命令
{
	u8 i;	
	
	LCD_DC_L;	//DC引脚输出低，代表写指令
	LCD_RS_L;	
	for(i=0;i<8;i++)
	{
		LCD_SCK_L;	
		
		if(com&0x80)
			LCD_SDA_H;
		else 
			LCD_SDA_L;
		
		LCD_SCK_H;	
		com = com<<=1;
	}
	LCD_DC_H;
}

/*****************************************************************************************
*	函 数 名: WriteData_7565
*	入口参数: dat - 需要写入的数据
*	返 回 值: 无
*	函数功能: 用于写入数据
*	说    明: 无
******************************************************************************************/

void WriteData_7565(u8 dat)//写数据
{
	u8 i;
	
	LCD_DC_L;		//DC引脚输出低，代表写数据
	LCD_RS_H;	
	for(i=0;i<8;i++)
	{
		LCD_SCK_L;	
		
		if(dat&0x80)
			LCD_SDA_H;
		else 
			LCD_SDA_L;
		
		LCD_SCK_H;	
		dat=dat<<=1;
	}	
	LCD_DC_H;
}

/*****************************************************************************************
*	函 数 名: LCD_Init
*	入口参数: 无
*	返 回 值: 无
*	函数功能: 初始化LCD屏幕，进行升压、对比度的配置
*	说    明: 因为屏幕的出场批次、工艺上会有些许差别，所以同样的对比度设置未必会是一样的显示效果，
*				 用户需要根据自己手中的屏幕来微调对比度，实际只需更改头文件 lcd_12864.h 里的宏定义
*				 LCD_Contrast的值，使屏幕显示的字迹达到最清晰的效果。
*				 
******************************************************************************************/

void LCD_Init(void)
{         
 	LCD_GPIO_Config();	// 初始化IO口

	LCD_RST_L; LCD_Delay(40);		// RST引脚输出低电平，LCD复位
	LCD_RST_H; LCD_Delay(40);		// RST输出高电平，完成复位
	
	WriteCom_7565(0xe2); LCD_Delay(5);	 	// 软复位
	WriteCom_7565(0x2c); LCD_Delay(5);	 	// 升压步聚1	
	WriteCom_7565(0x2e); LCD_Delay(5);	 	// 升压步聚2	
	WriteCom_7565(0x2f); LCD_Delay(5);	 	// 升压步聚3
	
	WriteCom_7565(0x24);	  			// 粗调对比度，可设置范围0x20～0x27
	WriteCom_7565(0x81);				// 对比度微调指令
	WriteCom_7565(LCD_Contrast);	// 微调对比度的值，可设置范围0x00～0x3f
	
	WriteCom_7565(0xa2);		// 1/9 偏压比（bias）
	WriteCom_7565(0xc8);		// 行扫描顺序：0XC0正向，0XC8逆向
	WriteCom_7565(0xa0);		// 列扫描顺序：0XA0正向，0XA1逆向
	WriteCom_7565(0x40);		// 起始值0X40：玻璃第一行的位置
	WriteCom_7565(0xaf);		// 开显示
	
	LCD_SetFont(&Font16);					// 设置默认字体1608
	LCD_ShowNumMode(Fill_Space);			// 数字显示填充空格	
	LCD_SetReverse(OFF);						// 反白关闭
 	LCD_SetCursor(0,Cursor_Underline);	// 不开光标
	LCD_ClearLine(0);
	LCD_Clear();								// 清除显示
}

/*****************************************************************************************
*	函 数 名:	 LCD_Address
*	入口参数:	 x - 水平坐标，取值范围 0~127
*				 y - 垂直坐标，取值范围 0~7
*	返 回 值: 无
*	函数功能: 设置需要显示数据的坐标
*	说    明: 12864的垂直坐标以8个像素点为单位，所以垂直坐标取值范围在0~7之间，也就是分8行显示
*				 			 
******************************************************************************************/

void LCD_Address(u8 x,u8 y)		
{
	y = y+4;
	if(y>= 8)
		y=y-8;
	WriteCom_7565(0xb0+y); 					// 设置垂直左边
	WriteCom_7565(((x>>4)&0x0f)+0x10); 	// 设置水平坐标的高4位
	WriteCom_7565(x&0x0f); 					// 设置水平坐标的低4位
}

/*****************************************************************************************
*	函 数 名:	 LCD_Clear
*	入口参数:	 无
*	返 回 值: 无
*	函数功能: 清除屏幕
*	说    明: 无
*				 			 
******************************************************************************************/

void LCD_Clear(void)		
{
	u8 i,j;
	
	for(i=1;i<8;i++)				
	{
		LCD_Address(0,0+i);			//设置坐标
		for(j=0;j<132;j++)
		{
			WriteData_7565(0x00);	//写入0x00，即不显示
		}
	}
}

void LCD_ClearLine(uint8_t line)		
{
	u8 j;	
	LCD_Address(0,line);			//设置坐标
	for(j=0;j<132;j++)
	{
		WriteData_7565(0x00);	//写入0x00，即不显示
	}
}


/*****************************************************************************************
*	函 数 名:	 LCD_SetFont
*	入口参数:	 *fonts - 需要使用的字体结构体，可选参数 Font32/Font24/Font16/Font08
*	返 回 值:	 无
*	函数功能:	 设置要显示的字体
*	说    明: 设置用于需要显示的字体，相关字体定义在lcd_fonts.h,可供使用的的ASCII字体有
*				 3216/2412/1608/0808，需要注意的是该函数无法设置汉字的字体，汉字默认使用1616的字体
*				 		 			 
******************************************************************************************/

void LCD_SetFont(pFONT *fonts)
{
  LCD_Fonts = fonts;
}

/*****************************************************************************************
*	函 数 名:	 LCD_SetReverse
*	入口参数:	 mode - 是否开启反白，可选值 ON / OFF
*	返 回 值:	 无
*	函数功能:	 开启反白，接下来显示的内容全部以反白的模式显示
*	说    明: 该函数通常用于菜单的设计
*					 		 			 
******************************************************************************************/

void LCD_SetReverse(u8 mode)
{
  LCD.Reverse_Mode = mode;
}

/*****************************************************************************************
*	函 数 名:	 LCD_SetCursor
*	入口参数:	 n - 光标位置，表示光标要出现在数字的第几位
*				 mode - 光标模式，可选值 Cursor_Reverse（反白光标） / Cursor_Underline（下划线）
*	返 回 值:	 无
*	函数功能:	 设置光标要显示的位置以及光标的模式
*	说    明: 该函数只在使用 LCD_DisplayNumber 进行数字变量的显示时有效，在将要显示的数字区域
*				 开启光标，若输入参数n为0，则不显示光标
*					 		 			 
******************************************************************************************/

void LCD_SetCursor(u8 n,u8 mode)
{
	LCD.Cursor_Location = n;	// 设置光标位
	LCD.Cursor_Mode = mode;		// 设置光标的模式，可选值 Cursor_Reverse / Cursor_Underline
}

/*****************************************************************************************
*	函 数 名:	 LCD_DisplayChar
*	入口参数:	 x - 水平坐标，取值范围 0~127
*				 y - 垂直坐标，取值范围 0~7
*				 ASCII - 要显示的ASCII码
*	返 回 值: 无
*	函数功能: 在指定坐标处显示单个ASCII码
*	说    明: 该函数无法用于显示汉字
*				 			 
******************************************************************************************/

void LCD_DisplayChar(u8 x, u8 y,u8 ASCII)
{
	u16  index = 0, counter = 0;
   u16  CharOffset;	// 字模的值

	ASCII = ASCII - 32; 	// 减去字库中没有的ASCII字符
	CharOffset = ASCII * LCD_Fonts->Sizes;	// 计算字模地址偏移量

	for(index = 0; index < ( LCD_Fonts->Height / 8 ); index++)
	{
		LCD_Address(x,y + index);	// 设置坐标
		
		for(counter = 0; counter < LCD_Fonts->Width; counter++)
		{	
			if(LCD.Reverse_Mode == ON)				// 判断是否开启了反白模式
			{
				WriteData_7565(~LCD_Fonts->table[ CharOffset + (counter + LCD_Fonts->Width*index) ]);		  	// 将要显示的数据取反
			}
			else if(LCD.Cursor_ShowFlag == ON)	// 判断是否需要显示光标
			{
				switch(LCD.Cursor_Mode)		
				{
					case Cursor_Reverse:		//反白模式光标
						WriteData_7565( ~LCD_Fonts->table[CharOffset + (counter + LCD_Fonts->Width*index)] );	// 将要显示的数据取反	
					break;
					case Cursor_Underline:	// 下划线模式		
						//将要显示的数据与下划线进行与运算
						WriteData_7565( LCD_Fonts->table[CharOffset + (counter + LCD_Fonts->Width*index)] | LCD_Fonts->table[63*LCD_Fonts->Sizes + (counter + LCD_Fonts->Width*index)] );		
					break;
				}
			}
			else	//默认情况下直接进行显示
			{
				WriteData_7565(LCD_Fonts->table[ CharOffset + (counter + LCD_Fonts->Width*index) ]);
			}
		}	
	}
	if( LCD.Cursor_ShowFlag == ON)	//判断光标是否已经显示
	{
		LCD.Cursor_ShowFlag = OFF;		
	}			
}

/*****************************************************************************************
*	函 数 名:	 LCD_DisplayString
*	入口参数:	 x - 水平坐标，取值范围 0~127
*				 y - 垂直坐标，取值范围 0~7
*				 *p - 要显示的ASCII字符串
*	返 回 值: 无
*	函数功能: 在指定坐标处显示ASCII字符串
*	说    明: 该函数无法用于显示汉字
*				 			 
******************************************************************************************/

void LCD_DisplayString( u8 x, u8 y, u8 *p) 
{
	while (*p != 0)		//判断是否为空字符
	{
		LCD_DisplayChar( x,y,*p);  //显示当前字符
		x += LCD_Fonts->Width; 		//改变坐标，用以显示下一个字符
		p++;		//取下一个字符地址
	}
}

/*****************************************************************************************
*	函 数 名:	 GetNumber
*	入口参数:	 number - 目标整数
*				 size - 目标数的长度
*	返 回 值: 取出的第N位的数
*	函数功能: 取整数的第N位的数，用于拆分整数以进行显示
*	说    明: 在 LCD_DisplayNumber 里被调用
*				 			 
******************************************************************************************/

u8 GetNumber(u32 num,u8 size)
{
	u32 numPow = 1;	
	u8  vaule;
	
	while(size>0)	
	{
		numPow *=10;
		size--;
	}
	vaule = (num/numPow)%10;
	return vaule;	
}

/*****************************************************************************************
*	函 数 名: LCD_ShowNumMode
*	入口参数: mode - 设置补0还是补空格，柯取值 Fill_Space（填充空格）/Fill_Zero（填充零）
*	返 回 值: 无
*	函数功能: 设置数字高位为0时的显示方式
*	说    明: 在使用 LCD_DisplayNumber 显示数字变量时，若高位是0，该函数可设置该0位是否显示，
*			    通常在显示时间的时候填充0
*				 			 
******************************************************************************************/

void LCD_ShowNumMode(u8 mode)
{
	LCD.ShowNum_Mode = mode;
}

/*****************************************************************************************
*	函 数 名:	 LCD_DisplayNumber
*	入口参数:	 x - 水平坐标，取值范围 0~127
*			  	 y - 垂直坐标，取值范围 0~7
*			    number - 要显示的数字，0~4294967295
*			  	 len - 数字的长度，即位数
*	返 回 值: 无
*	函数功能: 在指定坐标处显示数字变量，取值范围 0~4294967295
*	说    明: 使用该函数显示数字时需要指定长度
*				 			 
******************************************************************************************/

void LCD_DisplayNumber(u8 x, u8 y, u32 number, u8 len) 
{  
	u8 i,value;
	u8 zero_Flag = 0;		// 高位的0位标志

	for(i=0;i<len;i++)
	{
		value = GetNumber(number,len-i-1);	//获取当前整数的第N位数
		if( (LCD.Cursor_Location !=0) && (LCD.Cursor_Location == i+1) )	// 判断是否开启光标
		{
			LCD.Cursor_ShowFlag = ON;	// 光标显示位打开
			LCD.Cursor_Location = 0;	// 清除光标标志，光标只需显示一次
		}
		if( zero_Flag==0 && (i<len-1) )	//判断前面为0的部分
		{
			if(value == 0)
			{
				if(LCD.ShowNum_Mode == 0)	
					LCD_DisplayChar( x+i*LCD_Fonts->Width, y,'0');	//填充0
				else
					LCD_DisplayChar( x+i*LCD_Fonts->Width, y,' ');	//填充空格
				continue;
			}
			else
				zero_Flag = 1;	//当遇到第一个非0数时置1
		}									
		LCD_DisplayChar( x+i*LCD_Fonts->Width, y, value+48 );	//将拆分的数进行显示
	}
}

/*****************************************************************************************
*	函 数 名:	 LCD_DisplayChinese
*	入口参数:	 x - 水平坐标，取值范围 0~127
*			  	 y - 垂直坐标，取值范围 0~7
*			  	 *pText - 要显示的汉字
*	返 回 值: 无
*	函数功能: 在指定坐标处显示单个汉字
*	说    明: 该函数无法显示ASCII，同时汉字默认使用1616的字体，不支持改变字体
*				 			 
******************************************************************************************/

void LCD_DisplayChinese(u8 x, u8 y, u8 *pText) 
{
	u8  i=0,j=0,m=0;
	u16 addr;	// 字模地址

	while(1)
	{		
		if ( Chinese_Table[i]==*pText && Chinese_Table[i+1]==*(pText+1) )	// 对比数组中的汉字编码，用以定位该汉字字模的地址
		{   
			addr=i*16;	// 字模地址偏移
			break;
		}				
		i+=2;

		if(Chinese_Table[i] == 0)	break;	// 字模列表中无相应的汉字	
	}	
	for(j=0;j<2;j++)
	{		               
		LCD_Address(x,y + j);					
		for (m=0;m<16;m++)
		{	
			if(LCD.Reverse_Mode == ON)	// 如果开启反白
			{			
				WriteData_7565(~GBK2312_1212_Table[addr+m+16*j]);	// 字模取反
			}	
			else		
			{
				WriteData_7565(GBK2312_1212_Table[addr+m+16*j]);	//不开启反白则直接显示
			}
		}	
	}
}

/*****************************************************************************************
*	函 数 名:	 LCD_DisplayText
*	入口参数:	 x - 水平坐标，取值范围 0~127
*			  	 y - 垂直坐标，取值范围 0~7
*			  	 *pText - 要显示的汉字串
*	返 回 值: 无
*	函数功能: 在指定坐标处显示文本，可显示汉字或ASCII码
*	说    明: 1.使用该函数进行汉字显示时，需要先在lcd_fonts.c添加相应汉字字模，才能使用此函数显示
*				 汉字，不支持改变汉字的字体，默认使用1616的汉字。
*				 2.使用该函数显示标点符号时，需要把输入法切换到英文输入状态，否则无法正常显示
*
******************************************************************************************/

void LCD_DisplayText(u8 x, u8 y, u8 *pText) 
{  
 	
	while(*pText != 0)	// 判断是否为空字符
	{
		if(*pText<=0x80)	// 判断是否为ASCII码
		{
			LCD_DisplayChar(x,y,*pText);	// 显示ASCII
			if(LCD_Fonts->Width == 8){
				if(*pText == '.'|| *pText == ':')
					x+=5;
				else
					x+=7;
			}else if(LCD_Fonts->Width == 5){
				x+=6;
			}else{
				x+=LCD_Fonts->Width;				// 水平坐标调到下一个字符处
			}
			pText++;								// 字符串地址+1
		}
		else					// 若字符为汉字
		{			
			LCD_DisplayChinese(x,y,pText);	// 显示汉字
			x+=12;									// 水平坐标调到下一个字符处  字符取模14*14
			pText+=2;								// 字符串地址+2，汉字的编码需要16位的数据表示
		}
	}	
}

/*****************************************************************************************
*	函 数 名:	 LCD_DrawImage
*	入口参数:	 x - 水平坐标，取值范围 0~127
*			 			 y - 垂直坐标，取值范围 0~7
*			 			 width  - 图片的水平宽度，最大取值128
*						 height - 图片的垂直宽度，最大取值64
*	返 回 值: 无
*	函数功能: 在指定坐标处显示图片
*	说    明: 图片的垂直宽度取8的倍数，例如图片垂直宽度为38，此时应该输入40，
*		
*				 
******************************************************************************************/

void LCD_DrawImage(u8 x,u8 y,u8 width,u8 height,const u8 *pImage) 
{  
	u8 j=0,m=0;

	for(j=0;j<height/8;j++)		
	{
		LCD_Address(x,y + j);	// 设置坐标		
		for (m=0;m<width;m++)	
		{	
			if(LCD.Reverse_Mode == ON)	// 如果开启反白
			{	
				WriteData_7565(~pImage[m + width*j]);	// 图片取反				
			}				
			else
			{		
				WriteData_7565(pImage[m + width*j]);	// 不反白显示
			}
		}			
	}
}


/*****************************************************************************************
*	函 数 名:	 LCDClearArea
*	入口参数:	 x - 水平坐标，取值范围 0~127
*			 			 y - 垂直坐标，取值范围 0~7
*			 			 width  - 图片的水平宽度，最大取值128
*				 		 height - 图片的垂直宽度，最大取值64
*	返 回 值: 无
*	函数功能: 清除指定位置屏幕
*	说    明: 垂直宽度取8的倍数，例如垂直宽度为38，此时应该输入40，
*		
*				 
******************************************************************************************/

void LCDClearArea(u8 x,u8 y,u8 width,u8 height) 
{  
	u8 j=0,m=0;

	for(j=0;j<height/8;j++)		
	{
		LCD_Address(x,y + j);	// 设置坐标		
		for (m=0;m<width;m++)	
		{	
			if(LCD.Reverse_Mode == ON)	// 如果开启反白
			{	
				WriteData_7565(0xFF);	// 图片取反				
			}				
			else
			{		
				WriteData_7565(0x00);	// 不反白显示
			}
		}			
	}
}



