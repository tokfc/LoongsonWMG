#include "TouchLCD.h"
#include <stdio.h>

static sFONT *LCD_fonts  = NULL;//字体
static uint16 TextColor  ;//前景色
static uint16 BackColor  ;//背景色


void LCD_Delay (uint32 nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}


void LCDinit()
{
	GPIORegLoad();
	ILI9341_Init();
	XPT2046_Init();
	LCD_fonts=LCD_GetFont();
	TextColor=WHITE ;
	BackColor=BACKGROUND;
	LCD_SetColors(BLACK,BACKGROUND);
	ILI9341_Clear(0,0,320,240);	/* 清屏，显示全黄 */
}

void LCDRelease()
{
	LCD_SetColors(BLACK,WHITE);
	ILI9341_Clear(0,0,320,240);
	GPIORegRelease();
}

void LCDDispStringLine_EN( uint16 line,char * pStr )
{
	LCD_SetColors(BLACK,BACKGROUND);
	LCD_fonts=LCD_GetFont();
	ILI9341_DispStringLine_EN(line* LCD_fonts->Height , pStr );
	
}


void DrawPage1()
{
	
	uint32  byteCount, bitCount,fontLength;
	uint8 *Pfont;
	LCD_SetColors(BLACK,BACKGROUND);
	ILI9341_Clear(0,0,320,240);	/* 清屏，显示全黄 */
	TextColor=BLACK ;
	BackColor=BACKGROUND;
	//设置显示大标题****************************************************************
	
	LCD_fonts=&BIGTitle;
	//每个字模的字节数
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
		
	//字模首地址
	/*ascii码表偏移值乘以每个字模的字节数，求出字模的偏移位置*/
	Pfont = (uint8 *)&LCD_fonts->table[0];
	
	
	ILI9341_OpenWindow ( 48, 28, LCD_fonts->Width, LCD_fonts->Height);
	
	ILI9341_Write_Cmd ( CMD_SetPixel );			

	//按字节读取字模数据
	//由于前面直接设置了显示窗口，显示数据会自动换行
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		//一位一位处理要显示的颜色
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	//显示大标题***********************************************************************************
	
	//显示按钮，X,Y,长，高，填充TextColor ***********************************************************
	
	ILI9341_DrawRectangle(39,89,102,62,0 );
	ILI9341_DrawRectangle(179,89,102,62,0 );
	ILI9341_DrawRectangle(109,169,102,62,0 );
	ILI9341_DrawRectangle(0,198,42,42,0 );
	
	//按键填色
	LCD_SetTextColor(RGB24to16(0xBBFFFF));
	ILI9341_DrawRectangle(40,90,100,60,1 );
	ILI9341_DrawRectangle(180,90,100,60,1 );
	ILI9341_DrawRectangle(110,170,100,60,1 );
	LCD_SetTextColor(RED);
	ILI9341_DrawRectangle(1,199,40,40,1 );
	
	
	TextColor=BLACK;
	BackColor=RGB24to16(0xBBFFFF);
	
	
	LCD_fonts=&Button1;
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
	Pfont = (uint8 *)&LCD_fonts->table[0];
	ILI9341_OpenWindow ( 74, 110, LCD_fonts->Width, LCD_fonts->Height);
	ILI9341_Write_Cmd ( CMD_SetPixel );			
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	
	
	LCD_fonts=&Button2;
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
	Pfont = (uint8 *)&LCD_fonts->table[0];
	ILI9341_OpenWindow ( 214, 110, LCD_fonts->Width, LCD_fonts->Height);
	ILI9341_Write_Cmd ( CMD_SetPixel );			
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	
	
	LCD_fonts=&Button3;
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
	Pfont = (uint8 *)&LCD_fonts->table[0];
	ILI9341_OpenWindow ( 128, 190, LCD_fonts->Width, LCD_fonts->Height);
	ILI9341_Write_Cmd ( CMD_SetPixel );			
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	
	//显示按钮，X,Y,长，高，填充TextColor ***********************************************************
	LCD_SetColors(BLACK,BACKGROUND);
}

void DrawPage2()
{
	
	uint32  byteCount, bitCount,fontLength;
	uint8 *Pfont;
	LCD_SetColors(BLACK,BACKGROUND);
	ILI9341_Clear(0,0,320,240);	/* 清屏，显示全黄 */
	TextColor=BLACK ;
	BackColor=BACKGROUND;
	//设置显示大标题****************************************************************
	
	LCD_fonts=&BIGTitle;
	//每个字模的字节数
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
		
	//字模首地址
	/*ascii码表偏移值乘以每个字模的字节数，求出字模的偏移位置*/
	Pfont = (uint8 *)&LCD_fonts->table[0];
	
	
	ILI9341_OpenWindow ( 48, 28, LCD_fonts->Width, LCD_fonts->Height);
	
	ILI9341_Write_Cmd ( CMD_SetPixel );			

	//按字节读取字模数据
	//由于前面直接设置了显示窗口，显示数据会自动换行
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		//一位一位处理要显示的颜色
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	//显示大标题***********************************************************************************
	
	//显示输入提示*********************************************************************************
	LCD_fonts=&Tips1;
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
	Pfont = (uint8 *)&LCD_fonts->table[0];
	ILI9341_OpenWindow ( 104, 80, LCD_fonts->Width, LCD_fonts->Height);
	ILI9341_Write_Cmd ( CMD_SetPixel );
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	//显示输入提示*********************************************************************************
	 
	//显示按钮，X,Y,长，高，填充TextColor ***********************************************************
	
	ILI9341_DrawRectangle(39,109,102,62,0 );
	ILI9341_DrawRectangle(179,109,102,62,0 );
	ILI9341_DrawRectangle(0,198,42,42,0 );
	
	//按键填色
	LCD_SetTextColor(RGB24to16(0xBBFFFF));
	ILI9341_DrawRectangle(40,110,100,60,1 );
	ILI9341_DrawRectangle(180,110,100,60,1 );
	LCD_SetTextColor(RED);
	ILI9341_DrawRectangle(1,199,40,40,1 );
	
	
	TextColor=BLACK;
	BackColor=RGB24to16(0xBBFFFF);
	
	
	LCD_fonts=&Button4;
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
	Pfont = (uint8 *)&LCD_fonts->table[0];
	ILI9341_OpenWindow ( 74, 130, LCD_fonts->Width, LCD_fonts->Height);
	ILI9341_Write_Cmd ( CMD_SetPixel );			
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	
	
	LCD_fonts=&Button5;
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
	Pfont = (uint8 *)&LCD_fonts->table[0];
	ILI9341_OpenWindow ( 214, 130, LCD_fonts->Width, LCD_fonts->Height);
	ILI9341_Write_Cmd ( CMD_SetPixel );			
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	
	//显示按钮，X,Y,长，高，填充TextColor ***********************************************************
	LCD_SetColors(BLACK,BACKGROUND);
}

void DrawPage3() 
{
	LCD_SetColors(BLACK,BACKGROUND);
	ILI9341_Clear(0,0,320,240);	/* 清屏，显示全黄 */
	
	//返回键
	ILI9341_DrawRectangle(0,198,42,42,0 );
	LCD_SetTextColor(RED);
	ILI9341_DrawRectangle(1,199,40,40,1 );
	
	LCD_SetColors(BLACK,BACKGROUND);
}

void DrawPage4()
{
	
	uint32  byteCount, bitCount,fontLength;
	uint8 *Pfont;
	LCD_SetColors(BLACK,BACKGROUND);
	ILI9341_Clear(0,0,320,240);	/* 清屏，显示全黄 */
	TextColor=BLACK ;
	BackColor=BACKGROUND;
	//设置显示大标题****************************************************************
	
	LCD_fonts=&BIGTitle;
	//每个字模的字节数
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
		
	//字模首地址
	/*ascii码表偏移值乘以每个字模的字节数，求出字模的偏移位置*/
	Pfont = (uint8 *)&LCD_fonts->table[0];
	
	
	ILI9341_OpenWindow ( 48, 28, LCD_fonts->Width, LCD_fonts->Height);
	
	ILI9341_Write_Cmd ( CMD_SetPixel );			

	//按字节读取字模数据
	//由于前面直接设置了显示窗口，显示数据会自动换行
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		//一位一位处理要显示的颜色
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	//显示大标题***********************************************************************************
	
	//显示输入提示*********************************************************************************
	LCD_fonts=&Tips2;
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
	Pfont = (uint8 *)&LCD_fonts->table[0];
	ILI9341_OpenWindow ( 96, 80, LCD_fonts->Width, LCD_fonts->Height);
	ILI9341_Write_Cmd ( CMD_SetPixel );
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	//显示输入提示*********************************************************************************
	 
	//显示按钮，X,Y,长，高，填充TextColor ***********************************************************
	
	
	ILI9341_DrawRectangle(0,198,42,42,0 );
	ILI9341_DrawRectangle(278,198,42,42,0 );
	
	//按键填色
	LCD_SetTextColor(RGB24to16(0xBBFFFF));
	LCD_SetTextColor(RED);
	ILI9341_DrawRectangle(1,199,40,40,1 );
	LCD_SetTextColor(CYAN);
	ILI9341_DrawRectangle(279,199,40,40,1 );
	
	
	
	
	//显示按钮，X,Y,长，高，填充TextColor ***********************************************************
	LCD_SetColors(BLACK,BACKGROUND);
}

void DrawPage5()
{
	
	uint32  byteCount, bitCount,fontLength;
	uint8 *Pfont;
	LCD_SetColors(BLACK,BACKGROUND);
	ILI9341_Clear(0,0,320,240);	/* 清屏，显示全黄 */
	TextColor=BLACK ;
	BackColor=BACKGROUND;
	//设置显示大标题****************************************************************
	LCD_fonts=&BIGTitle;
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
	Pfont = (uint8 *)&LCD_fonts->table[0];
	ILI9341_OpenWindow ( 48, 10, LCD_fonts->Width, LCD_fonts->Height);
	ILI9341_Write_Cmd ( CMD_SetPixel );			
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		//一位一位处理要显示的颜色
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	//显示大标题***********************************************************************************
	
	//显示输入提示*********************************************************************************
	LCD_fonts=&Tips3;
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
	Pfont = (uint8 *)&LCD_fonts->table[0];
	ILI9341_OpenWindow ( 112, 54, LCD_fonts->Width, LCD_fonts->Height);
	ILI9341_Write_Cmd ( CMD_SetPixel );
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	//显示输入提示*********************************************************************************
	 
	//显示按钮，X,Y,长，高，填充TextColor ***********************************************************
	
	ILI9341_DrawRectangle(19,79,52,42,0 );
	ILI9341_DrawRectangle(79,79,52,42,0 );
	ILI9341_DrawRectangle(139,79,52,42,0 );
	ILI9341_DrawRectangle(199,79,52,42,0 );
	ILI9341_DrawRectangle(79,129,52,42,0 );
	ILI9341_DrawRectangle(139,129,52,42,0 );
	ILI9341_DrawRectangle(199,129,52,42,0 );
	ILI9341_DrawRectangle(79,179,52,42,0 );
	ILI9341_DrawRectangle(139,179,52,42,0 );
	ILI9341_DrawRectangle(199,179,52,42,0 );
	
	ILI9341_DrawRectangle(0,198,42,42,0 );
	ILI9341_DrawRectangle(278,198,42,42,0 );
	
	//按键填色
	LCD_SetTextColor(RGB24to16(0xBBFFFF));
	ILI9341_DrawRectangle(20,80,50,40,1 );
	ILI9341_DrawRectangle(80,80,50,40,1 );
	ILI9341_DrawRectangle(140,80,50,40,1 );
	ILI9341_DrawRectangle(200,80,50,40,1 );
	ILI9341_DrawRectangle(80,130,50,40,1 );
	ILI9341_DrawRectangle(140,130,50,40,1 );
	ILI9341_DrawRectangle(200,130,50,40,1 );
	ILI9341_DrawRectangle(80,180,50,40,1 );
	ILI9341_DrawRectangle(140,180,50,40,1 );
	ILI9341_DrawRectangle(200,180,50,40,1 );
	LCD_SetTextColor(RED);
	ILI9341_DrawRectangle(1,199,40,40,1 );
	LCD_SetTextColor(CYAN);
	ILI9341_DrawRectangle(279,199,40,40,1 );
	
	LCD_SetColors(BLACK,RGB24to16(0xBBFFFF));
	ILI9341_DispChar_EN(41, 92, '0' );
	ILI9341_DispChar_EN(101, 92, '1' );
	ILI9341_DispChar_EN(161, 92, '2' );
	ILI9341_DispChar_EN(221, 92, '3' );
	ILI9341_DispChar_EN(101, 142, '4' );
	ILI9341_DispChar_EN(161, 142, '5' );
	ILI9341_DispChar_EN(221, 142, '6' );
	ILI9341_DispChar_EN(101, 192, '7' );
	ILI9341_DispChar_EN(161, 192, '8' );
	ILI9341_DispChar_EN(221, 192, '9' );
	
	
	
	//显示按钮，X,Y,长，高，填充TextColor ***********************************************************
	LCD_SetColors(BLACK,BACKGROUND);
}

void DrawPage6() 
{
  uint32  byteCount, bitCount,fontLength;
	uint8 *Pfont;
	LCD_SetColors(BLACK,BACKGROUND);
	ILI9341_Clear(0,0,320,240);	/* 清屏，显示全黄 */
  TextColor=BLACK ;
	BackColor=BACKGROUND;
	//设置显示大标题****************************************************************
	LCD_fonts=&BIGTitle;
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
	Pfont = (uint8 *)&LCD_fonts->table[0];
	ILI9341_OpenWindow ( 48, 10, LCD_fonts->Width, LCD_fonts->Height);
	ILI9341_Write_Cmd ( CMD_SetPixel );			
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		//一位一位处理要显示的颜色
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
	//显示大标题************************************************************************************
  //显示二维码************************************************************************************
  LCD_SetColors(BLACK,WHITE);
  LCD_fonts=&LoongsonQRcode;
	fontLength = (LCD_fonts->Width*LCD_fonts->Height)/8;
	Pfont = (uint8 *)&LCD_fonts->table[0];
	ILI9341_OpenWindow ( 76, 50, LCD_fonts->Width, LCD_fonts->Height);
	ILI9341_Write_Cmd ( CMD_SetPixel );			
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
		//一位一位处理要显示的颜色
		for ( bitCount = 0; bitCount < 8; bitCount++ )
		{
			if ( Pfont[byteCount] & (0x80>>bitCount) )
			{
				ILI9341_Write_Data ( TextColor >> 8   );
				ILI9341_Write_Data ( TextColor & 0xff );
			}		
			else
			{
				ILI9341_Write_Data ( BackColor >> 8   );
				ILI9341_Write_Data ( BackColor & 0xff );
			}
			
		}	
	}
  LCD_SetColors(BLACK,BACKGROUND);
  //显示二维码************************************************************************************
	//返回键
	ILI9341_DrawRectangle(0,198,42,42,0 );
	LCD_SetTextColor(RED);
	ILI9341_DrawRectangle(1,199,40,40,1 );
	
	LCD_SetColors(BLACK,BACKGROUND);
}


char GetPage1()
{
	strType_XPT2046_Coordinate TouchedPoint;
	if(XPT2046_TouchDetect()==TOUCH_PRESSED)
	{
		//目前有效
		XPT2046_ReadAdc_Smooth_XY ( & TouchedPoint );
		//printf("Touched RealPoint(Cal) is ( %2d , %2d ) \n",TouchedPoint.x,TouchedPoint.y );
		
		//连续按下模式
		//LCD_Delay ( 0x01000000 );
		
		//释放按钮事件模式
		while(XPT2046_TouchDetect()==TOUCH_PRESSED) LCD_Delay ( 0x00200000 );
		
		// 取货(40,90,100,60,1 );
		// 存货(180,90,100,60,1 );
		// 查柜状态(110,170,100,60,1 );
		// No(1,199,40,40,1 );
		
		if(TouchedPoint.x>=40 && TouchedPoint.y>=90 	    && TouchedPoint.x<=140 && TouchedPoint.y<=150)	return 'a' ;
		else if(TouchedPoint.x>=180 && TouchedPoint.y>=90 	&& TouchedPoint.x<=280 && TouchedPoint.y<=150)	return 'b' ;
		else if(TouchedPoint.x>=110 && TouchedPoint.y>=170 	&& TouchedPoint.x<=210 && TouchedPoint.y<=230)	return 'c' ;
		else if(TouchedPoint.x>=1 && TouchedPoint.y>=199 	&& TouchedPoint.x<=41 && TouchedPoint.y<=239)	return 'S' ;
	}
	return 'P' ;
}


char GetPage2()
{
	strType_XPT2046_Coordinate TouchedPoint;
	if(XPT2046_TouchDetect()==TOUCH_PRESSED)
	{
		//目前有效
		XPT2046_ReadAdc_Smooth_XY ( & TouchedPoint );
		//printf("Touched RealPoint(Cal) is ( %2d , %2d ) \n",TouchedPoint.x,TouchedPoint.y );
		
		//连续按下模式
		//LCD_Delay ( 0x01000000 );
		
		//释放按钮事件模式
		while(XPT2046_TouchDetect()==TOUCH_PRESSED) LCD_Delay ( 0x00200000 );
		
		// 扫脸(40,110,100,60,1 );
		// 扫码(180,110,100,60,1 );
		// No(1,199,40,40,1 );
		if(TouchedPoint.x>=40 && TouchedPoint.y>=110 	    && TouchedPoint.x<=140 && TouchedPoint.y<=170)	return 'a' ;
		else if(TouchedPoint.x>=180 && TouchedPoint.y>=110 	&& TouchedPoint.x<=280 && TouchedPoint.y<=170)	return 'b' ;
		else if(TouchedPoint.x>=1 && TouchedPoint.y>=199 	&& TouchedPoint.x<=41 && TouchedPoint.y<=239)	return 'S' ;
	}
	return 'P' ;
}

char GetPage3()
{
	strType_XPT2046_Coordinate TouchedPoint;
	if(XPT2046_TouchDetect()==TOUCH_PRESSED)
	{
		//目前有效
		XPT2046_ReadAdc_Smooth_XY ( & TouchedPoint );
		//printf("Touched RealPoint(Cal) is ( %2d , %2d ) \n",TouchedPoint.x,TouchedPoint.y );
		
		//连续按下模式
		//LCD_Delay ( 0x01000000 );
		
		//释放按钮事件模式
		while(XPT2046_TouchDetect()==TOUCH_PRESSED) LCD_Delay ( 0x00200000 );
		
		// No(1,199,40,40,1 );
		if(TouchedPoint.x>=1 && TouchedPoint.y>=199 	&& TouchedPoint.x<=41 && TouchedPoint.y<=239)	return 'S' ;
	}
	return 'P' ;
}

char GetPage4()
{
	strType_XPT2046_Coordinate TouchedPoint;
	if(XPT2046_TouchDetect()==TOUCH_PRESSED)
	{
		//目前有效
		XPT2046_ReadAdc_Smooth_XY ( & TouchedPoint );
		//printf("Touched RealPoint(Cal) is ( %2d , %2d ) \n",TouchedPoint.x,TouchedPoint.y );
		
		//连续按下模式
		//LCD_Delay ( 0x01000000 );
		
		//释放按钮事件模式
		while(XPT2046_TouchDetect()==TOUCH_PRESSED) LCD_Delay ( 0x00200000 );
		
		// No(1,199,40,40,1 );
		// Yes(279,199,40,40,1 );
		if(TouchedPoint.x>=279 && TouchedPoint.y>=199 	    && TouchedPoint.x<=319 && TouchedPoint.y<=239)	return 'Y' ;
		else if(TouchedPoint.x>=1 && TouchedPoint.y>=199 	&& TouchedPoint.x<=41 && TouchedPoint.y<=239)	return 'N' ;
	}
	return 'P' ;
}

char GetPage5()
{
	strType_XPT2046_Coordinate TouchedPoint;
	if(XPT2046_TouchDetect()==TOUCH_PRESSED)
	{
		//目前有效
		XPT2046_ReadAdc_Smooth_XY ( & TouchedPoint );
		//printf("Touched RealPoint(Cal) is ( %2d , %2d ) \n",TouchedPoint.x,TouchedPoint.y );
		
		//连续按下模式
		//LCD_Delay ( 0x01000000 );
		
		//释放按钮事件模式
		while(XPT2046_TouchDetect()==TOUCH_PRESSED) LCD_Delay ( 0x00200000 );
		
		// No(1,199,40,40,1 );
		// Yes(279,199,40,40,1 );
		//'0'(20,80,50,40,1 );
		//'1'(80,80,50,40,1 );
		//'2'(140,80,50,40,1 );
		//'3'(200,80,50,40,1 );
		//'4'(80,130,50,40,1 );
		//'5'(140,130,50,40,1 );
		//'6'(200,130,50,40,1 );
		//'7'(80,180,50,40,1 );
		//'8'(140,180,50,40,1 );
		//'9'(200,180,50,40,1 );
		if	   (TouchedPoint.x>=20 	&& TouchedPoint.y>=80 	&& TouchedPoint.x<=70  && TouchedPoint.y<=120)	return '0' ;
		else if(TouchedPoint.x>=80  && TouchedPoint.y>=80 	&& TouchedPoint.x<=130 && TouchedPoint.y<=120)	return '1' ;
		else if(TouchedPoint.x>=140 && TouchedPoint.y>=80 	&& TouchedPoint.x<=190 && TouchedPoint.y<=120)	return '2' ;
		else if(TouchedPoint.x>=200 && TouchedPoint.y>=80 	&& TouchedPoint.x<=250 && TouchedPoint.y<=120)	return '3' ;
		else if(TouchedPoint.x>=80  && TouchedPoint.y>=130 	&& TouchedPoint.x<=130 && TouchedPoint.y<=170)	return '4' ;
		else if(TouchedPoint.x>=140 && TouchedPoint.y>=130 	&& TouchedPoint.x<=190 && TouchedPoint.y<=170)	return '5' ;
		else if(TouchedPoint.x>=200 && TouchedPoint.y>=130 	&& TouchedPoint.x<=250 && TouchedPoint.y<=170)	return '6' ;
		else if(TouchedPoint.x>=80  && TouchedPoint.y>=180 	&& TouchedPoint.x<=130 && TouchedPoint.y<=220)	return '7' ;
		else if(TouchedPoint.x>=140 && TouchedPoint.y>=180 	&& TouchedPoint.x<=190 && TouchedPoint.y<=220)	return '8' ;
		else if(TouchedPoint.x>=200 && TouchedPoint.y>=180 	&& TouchedPoint.x<=250 && TouchedPoint.y<=220)	return '9' ;
		else if(TouchedPoint.x>=279 && TouchedPoint.y>=180 	&& TouchedPoint.x<=319 && TouchedPoint.y<=239)	return 'Y' ;
		else if(TouchedPoint.x>=1   && TouchedPoint.y>=199 	&& TouchedPoint.x<=41  && TouchedPoint.y<=239)	return 'S' ;
	
	}
	return 'P' ;
}

char GetPage6()
{
	strType_XPT2046_Coordinate TouchedPoint;
	if(XPT2046_TouchDetect()==TOUCH_PRESSED)
	{
		//目前有效
		XPT2046_ReadAdc_Smooth_XY ( & TouchedPoint );
		//printf("Touched RealPoint(Cal) is ( %2d , %2d ) \n",TouchedPoint.x,TouchedPoint.y );
		
		//连续按下模式
		//LCD_Delay ( 0x01000000 );
		
		//释放按钮事件模式
		while(XPT2046_TouchDetect()==TOUCH_PRESSED) LCD_Delay ( 0x00200000 );
		
		// No(1,199,40,40,1 );
		if(TouchedPoint.x>=1 && TouchedPoint.y>=199 	&& TouchedPoint.x<=41 && TouchedPoint.y<=239)	return 'S' ;
	}
	return 'P' ;
}

