#include "ILI9341.h"
#include <stdio.h>

//根据液晶扫描方向而变化的XY像素宽度
//调用ILI9341_GramScan函数设置方向时会自动更改
uint16 LCD_X_LENGTH = ILI9341_LESS_PIXEL;
uint16 LCD_Y_LENGTH = ILI9341_MORE_PIXEL;

//液晶屏扫描模式，本变量主要用于方便选择触摸屏的计算参数
//参数可选值为0-7
//调用ILI9341_GramScan函数设置方向时会自动更改
//LCD刚初始化完成时会使用本默认值
uint8 LCD_SCAN_MODE = 5;


static sFONT *LCD_Currentfonts  = &FontEN8x16;  //英文字体
static uint16 CurrentTextColor  = BLACK;//前景色
static uint16 CurrentBackColor  = WHITE;//背景色

void ILI9341_Delay (uint32 nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}


void ILI9341_GPIO_Config()
{
  *GPIO_OEN=*GPIO_OEN&(~(LCDCS|LCDRS|LCDSCL|LCDSDI|LCDreset));       //控制线设置输出模式
  *GPIO_OEN=*GPIO_OEN|(LCDSDO);
  LCD_CS_1; //位选线闲时置1
  LCD_RS_1;
  LCD_SCL_0;
  LCD_SDI_1;
  LCD_reset_1;
  //printf("GPIO_OEN = %016llx \n",*GPIO_OEN);
  //printf("GPIO_OUT = %016llx \n",*GPIO_OUT);
  
}

void SPI_WR(uint8 data)
{
  uint8 counter1;
  //printf("Write SPI");
  for(counter1=0;counter1<8;counter1++)
  {
    LCD_SCL_0;
    if((data & (0x80>>counter1))==(0x80>>counter1)) {LCD_SDI_1;/*printf("1");*/}
	else {LCD_SDI_0;/*printf("0");*/}
    LCD_SCL_1;
  }
  //printf("b\n");
}

uint8 SPI_RD()
{
  uint8 tmp=0;
  uint8 counter1;
  //printf("Read SPI");
  for(counter1=0;counter1<8;counter1++)
  {
    LCD_SCL_0;
	if((*GPIO_IN)&LCDSDO == LCDSDO ) {tmp|=(0x01<<(7-counter1));/*printf("1");*/}
	//else printf("0");
	LCD_SCL_1;
  }
  //printf("b\n");
  return tmp;
}

//写命令（寄存器地址）操作，CS置零，RS置0
void ILI9341_Write_Cmd(uint8 Cmd)
{ 
  LCD_RS_0;//写命令(寄存器地址) 
  
  LCD_CS_0;
  SPI_WR(Cmd);
  LCD_CS_1;
}

//写数据操作，CS置零，RS置1
void ILI9341_Write_Data(uint8 data)
{
   LCD_RS_1; //写数据
   
   LCD_CS_0;
   SPI_WR(data);
   LCD_CS_1;
}

void ILI9341_REG_Config ( void )
{
	ILI9341_Write_Cmd(0xCF);  
	ILI9341_Write_Data(0x00); 
	ILI9341_Write_Data(0xD9); //0xC1 
	ILI9341_Write_Data(0X30); 
	ILI9341_Write_Cmd(0xED);  
	ILI9341_Write_Data(0x64); 
	ILI9341_Write_Data(0x03); 
	ILI9341_Write_Data(0X12); 
	ILI9341_Write_Data(0X81); 
	ILI9341_Write_Cmd(0xE8);  
	ILI9341_Write_Data(0x85); 
	ILI9341_Write_Data(0x10); 
	ILI9341_Write_Data(0x7A); 
	ILI9341_Write_Cmd(0xCB);  
	ILI9341_Write_Data(0x39); 
	ILI9341_Write_Data(0x2C); 
	ILI9341_Write_Data(0x00); 
	ILI9341_Write_Data(0x34); 
	ILI9341_Write_Data(0x02); 
	ILI9341_Write_Cmd(0xF7);  
	ILI9341_Write_Data(0x20); 
	ILI9341_Write_Cmd(0xEA);  
	ILI9341_Write_Data(0x00); 
	ILI9341_Write_Data(0x00); 
	ILI9341_Write_Cmd(0xC0);    //Power control 
	ILI9341_Write_Data(0x1B);   //VRH[5:0] 
	ILI9341_Write_Cmd(0xC1);    //Power control 
	ILI9341_Write_Data(0x12);   //SAP[2:0];BT[3:0] 0x01
	ILI9341_Write_Cmd(0xC5);    //VCM control 
	ILI9341_Write_Data(0x08); 	 //30
	ILI9341_Write_Data(0x26); 	 //30
	ILI9341_Write_Cmd(0xC7);    //VCM control2 
	ILI9341_Write_Data(0XB7); 
	ILI9341_Write_Cmd(0x36);    // Memory Access Control 
	ILI9341_Write_Data(0x48); //0x40
	ILI9341_Write_Cmd(0x3A);   
	ILI9341_Write_Data(0x55); 
	ILI9341_Write_Cmd(0xB1);   
	ILI9341_Write_Data(0x00);   
	ILI9341_Write_Data(0x1A); 
	ILI9341_Write_Cmd(0xB6);    // Display Function Control 
	ILI9341_Write_Data(0x0A); 
	ILI9341_Write_Data(0x22);//0xA2 
	ILI9341_Write_Cmd(0xF2);    // 3Gamma Function Disable 
	ILI9341_Write_Data(0x00); 
	ILI9341_Write_Cmd(0x26);    //Gamma curve selected 
	ILI9341_Write_Data(0x01); 
	ILI9341_Write_Cmd(0xE0);    //Set Gamma 
	ILI9341_Write_Data(0x0F); 
	ILI9341_Write_Data(0x1D); 
	ILI9341_Write_Data(0x1A); 
	ILI9341_Write_Data(0x0A); 
	ILI9341_Write_Data(0x0D); 
	ILI9341_Write_Data(0x07); 
	ILI9341_Write_Data(0x49); 
	ILI9341_Write_Data(0X66); 
	ILI9341_Write_Data(0x3B); 
	ILI9341_Write_Data(0x07); 
	ILI9341_Write_Data(0x11); 
	ILI9341_Write_Data(0x01); 
	ILI9341_Write_Data(0x09); 
	ILI9341_Write_Data(0x05); 
	ILI9341_Write_Data(0x04); 		 
	ILI9341_Write_Cmd(0XE1);    //Set Gamma 
	ILI9341_Write_Data(0x00); 
	ILI9341_Write_Data(0x18); 
	ILI9341_Write_Data(0x1D); 
	ILI9341_Write_Data(0x02); 
	ILI9341_Write_Data(0x0F); 
	ILI9341_Write_Data(0x04); 
	ILI9341_Write_Data(0x36); 
	ILI9341_Write_Data(0x13); 
	ILI9341_Write_Data(0x4C); 
	ILI9341_Write_Data(0x07); 
	ILI9341_Write_Data(0x13); 
	ILI9341_Write_Data(0x0F); 
	ILI9341_Write_Data(0x2E); 
	ILI9341_Write_Data(0x2F); 
	ILI9341_Write_Data(0x05); 

	
	/* column address control set */
	ILI9341_Write_Cmd ( CMD_SetCoordinateX ); 
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0xEF );
	
	/* page address control set */
	ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x00 );
	ILI9341_Write_Data ( 0x01 );
	ILI9341_Write_Data ( 0x3F );
	
	
	/* Sleep Out (11h)  */
	ILI9341_Write_Cmd ( 0x11 );	
	ILI9341_Delay ( 0xFFFF );

	
	/* Display ON (29h) */
	ILI9341_Write_Cmd ( 0x29 );  
	
	// /*Read ID*/
	// uint32 ID = ILI9341_Read_24Data(0xD3);
	// printf("ID1= 0x%08x \n",ID);
	
}

void ILI9341_Rst ( void )
{			
	LCD_reset_0;	 //低电平复位

	ILI9341_Delay ( 0xFFFFFF ); 					   

	LCD_reset_1;		 	 

	ILI9341_Delay ( 0xFFFFFF ); 	
	
}

//设置显示屏的旋转方向
void ILI9341_GramScan ( uint8 ucOption )
{	
	//参数检查，只可输入0-7
	if(ucOption >7 )
		return;
	
	//根据模式更新LCD_SCAN_MODE的值，主要用于触摸屏选择计算参数
	LCD_SCAN_MODE = ucOption;
	
	//根据模式更新XY方向的像素宽度
	if(ucOption%2 == 0)	
	{
		//0 2 4 6模式下X方向像素宽度为240，Y方向为320
		LCD_X_LENGTH = ILI9341_LESS_PIXEL;
		LCD_Y_LENGTH =	ILI9341_MORE_PIXEL;
	}
	else				
	{
		//1 3 5 7模式下X方向像素宽度为320，Y方向为240
		LCD_X_LENGTH = ILI9341_MORE_PIXEL;
		LCD_Y_LENGTH =	ILI9341_LESS_PIXEL; 
	}

	//0x36命令参数的高3位可用于设置GRAM扫描方向	
	ILI9341_Write_Cmd ( 0x36 ); 
	ILI9341_Write_Data ( 0x08 |(ucOption<<5));//根据ucOption的值设置LCD参数，共0-7种模式
	ILI9341_Write_Cmd ( CMD_SetCoordinateX ); 
	ILI9341_Write_Data ( 0x00 );		/* x 起始坐标高8位 */
	ILI9341_Write_Data ( 0x00 );		/* x 起始坐标低8位 */
	ILI9341_Write_Data ( ((LCD_X_LENGTH-1)>>8)&0xFF ); /* x 结束坐标高8位 */	
	ILI9341_Write_Data ( (LCD_X_LENGTH-1)&0xFF );				/* x 结束坐标低8位 */

	ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 
	ILI9341_Write_Data ( 0x00 );		/* y 起始坐标高8位 */
	ILI9341_Write_Data ( 0x00 );		/* y 起始坐标低8位 */
	ILI9341_Write_Data ( ((LCD_Y_LENGTH-1)>>8)&0xFF );	/* y 结束坐标高8位 */	 
	ILI9341_Write_Data ( (LCD_Y_LENGTH-1)&0xFF );				/* y 结束坐标低8位 */

	/* write gram start */
	ILI9341_Write_Cmd ( CMD_SetPixel );	
}

void ILI9341_Init ( void )
{
	ILI9341_GPIO_Config ();
	ILI9341_Rst ();
	ILI9341_REG_Config ();

	//设置默认扫描方向，其中 6 模式为大部分液晶例程的默认显示方向  
	ILI9341_GramScan(LCD_SCAN_MODE);
}

//打开一定范围的指针MAP
void ILI9341_OpenWindow ( uint16 usX, uint16 usY, uint16 usWidth, uint16 usHeight )
{	
	ILI9341_Write_Cmd ( CMD_SetCoordinateX ); 				 /* 设置X坐标 */
	ILI9341_Write_Data ( usX >> 8  );	 /* 先高8位，然后低8位 */
	ILI9341_Write_Data ( usX & 0xff  );	 /* 设置起始点和结束点*/
	ILI9341_Write_Data ( ( usX + usWidth - 1 ) >> 8  );
	ILI9341_Write_Data ( ( usX + usWidth - 1 ) & 0xff  );

	ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 			     /* 设置Y坐标*/
	ILI9341_Write_Data ( usY >> 8  );
	ILI9341_Write_Data ( usY & 0xff  );
	ILI9341_Write_Data ( ( usY + usHeight - 1 ) >> 8 );
	ILI9341_Write_Data ( ( usY + usHeight - 1) & 0xff );
	
}

//打开某个像素的指针
void ILI9341_SetCursor ( uint16 usX, uint16 usY )	
{
	ILI9341_OpenWindow ( usX, usY, 1, 1 );
}
//已打开的范围内填充usColor
void ILI9341_FillColor ( uint32 ulAmout_Point, uint16 usColor )
{
	uint32 i = 0;
	
	
	/* memory write */
	ILI9341_Write_Cmd ( CMD_SetPixel );	
		
	for ( i = 0; i < ulAmout_Point; i ++ )
	{
		ILI9341_Write_Data ( usColor >> 8 );
		ILI9341_Write_Data ( usColor & 0xff );
	}
		
	
}

//范围内清屏
void ILI9341_Clear ( uint16 usX, uint16 usY, uint16 usWidth, uint16 usHeight )
{
	ILI9341_OpenWindow ( usX, usY, usWidth, usHeight );

	ILI9341_FillColor ( usWidth * usHeight, CurrentBackColor );		
	
}

//某点填色,填CurrentTextColor
void ILI9341_SetPointPixel ( uint16 usX, uint16 usY )	
{	
	if ( ( usX < LCD_X_LENGTH ) && ( usY < LCD_Y_LENGTH ) )
  {
		ILI9341_SetCursor ( usX, usY );
		
		ILI9341_FillColor ( 1, CurrentTextColor );
	}
	
}


//画一条线，颜色跟SetPointPixel有关
void ILI9341_DrawLine ( uint16 usX1, uint16 usY1, uint16 usX2, uint16 usY2 )
{
	uint16 us; 
	uint16 usX_Current, usY_Current;
	
	int lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance; 
	int lIncrease_X, lIncrease_Y; 	
	
	
	lDelta_X = usX2 - usX1; //计算坐标增量 
	lDelta_Y = usY2 - usY1; 
	
	usX_Current = usX1; 
	usY_Current = usY1; 
	
	
	if ( lDelta_X > 0 ) 
		lIncrease_X = 1; //设置单步方向 
	
	else if ( lDelta_X == 0 ) 
		lIncrease_X = 0;//垂直线 
	
	else 
  { 
    lIncrease_X = -1;
    lDelta_X = - lDelta_X;
  } 

	
	if ( lDelta_Y > 0 )
		lIncrease_Y = 1; 
	
	else if ( lDelta_Y == 0 )
		lIncrease_Y = 0;//水平线 
	
	else 
  {
    lIncrease_Y = -1;
    lDelta_Y = - lDelta_Y;
  } 

	
	if (  lDelta_X > lDelta_Y )
		lDistance = lDelta_X; //选取基本增量坐标轴 
	
	else 
		lDistance = lDelta_Y; 

	
	for ( us = 0; us <= lDistance + 1; us ++ )//画线输出 
	{  
		ILI9341_SetPointPixel ( usX_Current, usY_Current );//画点 
		
		lError_X += lDelta_X ; 
		lError_Y += lDelta_Y ; 
		
		if ( lError_X > lDistance ) 
		{ 
			lError_X -= lDistance; 
			usX_Current += lIncrease_X; 
		}  
		
		if ( lError_Y > lDistance ) 
		{ 
			lError_Y -= lDistance; 
			usY_Current += lIncrease_Y; 
		} 
		
	}  
	
	
}   
//画一个矩形，filled决定是否填充
void ILI9341_DrawRectangle ( uint16 usX_Start, uint16 usY_Start, uint16 usWidth, uint16 usHeight, uint8 ucFilled )
{
	if ( ucFilled )
	{
		ILI9341_OpenWindow ( usX_Start, usY_Start, usWidth, usHeight );
		ILI9341_FillColor ( usWidth * usHeight ,CurrentTextColor);	
	}
	else
	{
		ILI9341_DrawLine ( usX_Start, usY_Start, usX_Start + usWidth - 1, usY_Start );
		ILI9341_DrawLine ( usX_Start, usY_Start + usHeight - 1, usX_Start + usWidth - 1, usY_Start + usHeight - 1 );
		ILI9341_DrawLine ( usX_Start, usY_Start, usX_Start, usY_Start + usHeight - 1 );
		ILI9341_DrawLine ( usX_Start + usWidth - 1, usY_Start, usX_Start + usWidth - 1, usY_Start + usHeight - 1 );		
	}

}

void ILI9341_DispChar_EN ( uint16 usX, uint16 usY, const char cChar )
{
	uint8  byteCount, bitCount,fontLength;	
	uint16 ucRelativePositon;
	uint8 *Pfont;
	
	//对ascii码表偏移（字模表不包含ASCII表的前32个非图形符号）
	ucRelativePositon = cChar - ' ';
	
	//每个字模的字节数
	fontLength = (LCD_Currentfonts->Width*LCD_Currentfonts->Height)/8;
		
	//字模首地址
	/*ascii码表偏移值乘以每个字模的字节数，求出字模的偏移位置*/
	Pfont = (uint8 *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];
	
	//设置显示窗口
	ILI9341_OpenWindow ( usX, usY, LCD_Currentfonts->Width, LCD_Currentfonts->Height);
	
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
						ILI9341_Write_Data ( CurrentTextColor >> 8   );
						ILI9341_Write_Data ( CurrentTextColor & 0xff );
					}
					
					else
					{
						ILI9341_Write_Data ( CurrentBackColor >> 8   );
						ILI9341_Write_Data ( CurrentBackColor & 0xff );
					}
					
			}	
	}	
}

void ILI9341_DispStringLine_EN (  uint16 line,  char * pStr )
{
	uint16 usX = 0;
	LCD_ClearLine(line);
	while ( * pStr != '\0' )
	{
		if ( ( usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width ) > LCD_X_LENGTH )
		{
			usX = ILI9341_DispWindow_X_Star;
			line += LCD_Currentfonts->Height;
		}
		
		if ( ( line - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH )
		{
			usX = ILI9341_DispWindow_X_Star;
			line = ILI9341_DispWindow_Y_Star;
		}


		ILI9341_DispChar_EN ( usX, line, * pStr);
		
		pStr ++;
		
		usX += LCD_Currentfonts->Width;
		
	}
  
}

void LCD_ClearLine(uint16 Line)
{
  ILI9341_Clear(0,Line,LCD_X_LENGTH,(LCD_Currentfonts->Height));	/* 清屏，显示全黑 */

}

void ILI9341_DispString_EN ( 	uint16 usX ,uint16 usY,  char * pStr )
{
	while ( * pStr != '\0' )
	{
		if ( ( usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width ) > LCD_X_LENGTH )
		{
			usX = ILI9341_DispWindow_X_Star;
			usY += LCD_Currentfonts->Height;
		}
		
		if ( ( usY - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height ) > LCD_Y_LENGTH )
		{
			usX = ILI9341_DispWindow_X_Star;
			usY = ILI9341_DispWindow_Y_Star;
		}
		
		ILI9341_DispChar_EN ( usX, usY, * pStr);
		
		pStr ++;
		
		usX += LCD_Currentfonts->Width;
		
	}
	
}

void ILI9341_DispString_EN_YDir (	 uint16 usX,uint16 usY ,  char * pStr )
{	
	while ( * pStr != '\0' )
	{
		if ( ( usY - ILI9341_DispWindow_Y_Star + LCD_Currentfonts->Height ) >LCD_Y_LENGTH  )
		{
			usY = ILI9341_DispWindow_Y_Star;
			usX += LCD_Currentfonts->Width;
		}
		
		if ( ( usX - ILI9341_DispWindow_X_Star + LCD_Currentfonts->Width ) >  LCD_X_LENGTH)
		{
			usX = ILI9341_DispWindow_X_Star;
			usY = ILI9341_DispWindow_Y_Star;
		}
		
		ILI9341_DispChar_EN ( usX, usY, * pStr);
		
		pStr ++;
		
		usY += LCD_Currentfonts->Height;		
	}	
}


void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}

sFONT *LCD_GetFont(void)
{
  return LCD_Currentfonts;
}

void LCD_SetColors(uint16 TextColor, uint16 BackColor) 
{
  CurrentTextColor = TextColor; 
  CurrentBackColor = BackColor;
}

void LCD_GetColors(uint16 *TextColor, uint16 *BackColor)
{
  *TextColor = CurrentTextColor;
  *BackColor = CurrentBackColor;
}

void LCD_SetTextColor(uint16 Color)
{
  CurrentTextColor = Color;
}

void LCD_SetBackColor(uint16 Color)
{
  CurrentBackColor = Color;
}

