#include "ILI9341.h"
#include <stdio.h>

//����Һ��ɨ�跽����仯��XY���ؿ��
//����ILI9341_GramScan�������÷���ʱ���Զ�����
uint16 LCD_X_LENGTH = ILI9341_LESS_PIXEL;
uint16 LCD_Y_LENGTH = ILI9341_MORE_PIXEL;

//Һ����ɨ��ģʽ����������Ҫ���ڷ���ѡ�������ļ������
//������ѡֵΪ0-7
//����ILI9341_GramScan�������÷���ʱ���Զ�����
//LCD�ճ�ʼ�����ʱ��ʹ�ñ�Ĭ��ֵ
uint8 LCD_SCAN_MODE = 5;


static sFONT *LCD_Currentfonts  = &FontEN8x16;  //Ӣ������
static uint16 CurrentTextColor  = BLACK;//ǰ��ɫ
static uint16 CurrentBackColor  = WHITE;//����ɫ

void ILI9341_Delay (uint32 nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}


void ILI9341_GPIO_Config()
{
  *GPIO_OEN=*GPIO_OEN&(~(LCDCS|LCDRS|LCDSCL|LCDSDI|LCDreset));       //�������������ģʽ
  *GPIO_OEN=*GPIO_OEN|(LCDSDO);
  LCD_CS_1; //λѡ����ʱ��1
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

//д����Ĵ�����ַ��������CS���㣬RS��0
void ILI9341_Write_Cmd(uint8 Cmd)
{ 
  LCD_RS_0;//д����(�Ĵ�����ַ) 
  
  LCD_CS_0;
  SPI_WR(Cmd);
  LCD_CS_1;
}

//д���ݲ�����CS���㣬RS��1
void ILI9341_Write_Data(uint8 data)
{
   LCD_RS_1; //д����
   
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
	LCD_reset_0;	 //�͵�ƽ��λ

	ILI9341_Delay ( 0xFFFFFF ); 					   

	LCD_reset_1;		 	 

	ILI9341_Delay ( 0xFFFFFF ); 	
	
}

//������ʾ������ת����
void ILI9341_GramScan ( uint8 ucOption )
{	
	//������飬ֻ������0-7
	if(ucOption >7 )
		return;
	
	//����ģʽ����LCD_SCAN_MODE��ֵ����Ҫ���ڴ�����ѡ��������
	LCD_SCAN_MODE = ucOption;
	
	//����ģʽ����XY��������ؿ��
	if(ucOption%2 == 0)	
	{
		//0 2 4 6ģʽ��X�������ؿ��Ϊ240��Y����Ϊ320
		LCD_X_LENGTH = ILI9341_LESS_PIXEL;
		LCD_Y_LENGTH =	ILI9341_MORE_PIXEL;
	}
	else				
	{
		//1 3 5 7ģʽ��X�������ؿ��Ϊ320��Y����Ϊ240
		LCD_X_LENGTH = ILI9341_MORE_PIXEL;
		LCD_Y_LENGTH =	ILI9341_LESS_PIXEL; 
	}

	//0x36��������ĸ�3λ����������GRAMɨ�跽��	
	ILI9341_Write_Cmd ( 0x36 ); 
	ILI9341_Write_Data ( 0x08 |(ucOption<<5));//����ucOption��ֵ����LCD��������0-7��ģʽ
	ILI9341_Write_Cmd ( CMD_SetCoordinateX ); 
	ILI9341_Write_Data ( 0x00 );		/* x ��ʼ�����8λ */
	ILI9341_Write_Data ( 0x00 );		/* x ��ʼ�����8λ */
	ILI9341_Write_Data ( ((LCD_X_LENGTH-1)>>8)&0xFF ); /* x ���������8λ */	
	ILI9341_Write_Data ( (LCD_X_LENGTH-1)&0xFF );				/* x ���������8λ */

	ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 
	ILI9341_Write_Data ( 0x00 );		/* y ��ʼ�����8λ */
	ILI9341_Write_Data ( 0x00 );		/* y ��ʼ�����8λ */
	ILI9341_Write_Data ( ((LCD_Y_LENGTH-1)>>8)&0xFF );	/* y ���������8λ */	 
	ILI9341_Write_Data ( (LCD_Y_LENGTH-1)&0xFF );				/* y ���������8λ */

	/* write gram start */
	ILI9341_Write_Cmd ( CMD_SetPixel );	
}

void ILI9341_Init ( void )
{
	ILI9341_GPIO_Config ();
	ILI9341_Rst ();
	ILI9341_REG_Config ();

	//����Ĭ��ɨ�跽������ 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
	ILI9341_GramScan(LCD_SCAN_MODE);
}

//��һ����Χ��ָ��MAP
void ILI9341_OpenWindow ( uint16 usX, uint16 usY, uint16 usWidth, uint16 usHeight )
{	
	ILI9341_Write_Cmd ( CMD_SetCoordinateX ); 				 /* ����X���� */
	ILI9341_Write_Data ( usX >> 8  );	 /* �ȸ�8λ��Ȼ���8λ */
	ILI9341_Write_Data ( usX & 0xff  );	 /* ������ʼ��ͽ�����*/
	ILI9341_Write_Data ( ( usX + usWidth - 1 ) >> 8  );
	ILI9341_Write_Data ( ( usX + usWidth - 1 ) & 0xff  );

	ILI9341_Write_Cmd ( CMD_SetCoordinateY ); 			     /* ����Y����*/
	ILI9341_Write_Data ( usY >> 8  );
	ILI9341_Write_Data ( usY & 0xff  );
	ILI9341_Write_Data ( ( usY + usHeight - 1 ) >> 8 );
	ILI9341_Write_Data ( ( usY + usHeight - 1) & 0xff );
	
}

//��ĳ�����ص�ָ��
void ILI9341_SetCursor ( uint16 usX, uint16 usY )	
{
	ILI9341_OpenWindow ( usX, usY, 1, 1 );
}
//�Ѵ򿪵ķ�Χ�����usColor
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

//��Χ������
void ILI9341_Clear ( uint16 usX, uint16 usY, uint16 usWidth, uint16 usHeight )
{
	ILI9341_OpenWindow ( usX, usY, usWidth, usHeight );

	ILI9341_FillColor ( usWidth * usHeight, CurrentBackColor );		
	
}

//ĳ����ɫ,��CurrentTextColor
void ILI9341_SetPointPixel ( uint16 usX, uint16 usY )	
{	
	if ( ( usX < LCD_X_LENGTH ) && ( usY < LCD_Y_LENGTH ) )
  {
		ILI9341_SetCursor ( usX, usY );
		
		ILI9341_FillColor ( 1, CurrentTextColor );
	}
	
}


//��һ���ߣ���ɫ��SetPointPixel�й�
void ILI9341_DrawLine ( uint16 usX1, uint16 usY1, uint16 usX2, uint16 usY2 )
{
	uint16 us; 
	uint16 usX_Current, usY_Current;
	
	int lError_X = 0, lError_Y = 0, lDelta_X, lDelta_Y, lDistance; 
	int lIncrease_X, lIncrease_Y; 	
	
	
	lDelta_X = usX2 - usX1; //������������ 
	lDelta_Y = usY2 - usY1; 
	
	usX_Current = usX1; 
	usY_Current = usY1; 
	
	
	if ( lDelta_X > 0 ) 
		lIncrease_X = 1; //���õ������� 
	
	else if ( lDelta_X == 0 ) 
		lIncrease_X = 0;//��ֱ�� 
	
	else 
  { 
    lIncrease_X = -1;
    lDelta_X = - lDelta_X;
  } 

	
	if ( lDelta_Y > 0 )
		lIncrease_Y = 1; 
	
	else if ( lDelta_Y == 0 )
		lIncrease_Y = 0;//ˮƽ�� 
	
	else 
  {
    lIncrease_Y = -1;
    lDelta_Y = - lDelta_Y;
  } 

	
	if (  lDelta_X > lDelta_Y )
		lDistance = lDelta_X; //ѡȡ�������������� 
	
	else 
		lDistance = lDelta_Y; 

	
	for ( us = 0; us <= lDistance + 1; us ++ )//������� 
	{  
		ILI9341_SetPointPixel ( usX_Current, usY_Current );//���� 
		
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
//��һ�����Σ�filled�����Ƿ����
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
	
	//��ascii���ƫ�ƣ���ģ������ASCII���ǰ32����ͼ�η��ţ�
	ucRelativePositon = cChar - ' ';
	
	//ÿ����ģ���ֽ���
	fontLength = (LCD_Currentfonts->Width*LCD_Currentfonts->Height)/8;
		
	//��ģ�׵�ַ
	/*ascii���ƫ��ֵ����ÿ����ģ���ֽ����������ģ��ƫ��λ��*/
	Pfont = (uint8 *)&LCD_Currentfonts->table[ucRelativePositon * fontLength];
	
	//������ʾ����
	ILI9341_OpenWindow ( usX, usY, LCD_Currentfonts->Width, LCD_Currentfonts->Height);
	
	ILI9341_Write_Cmd ( CMD_SetPixel );			

	//���ֽڶ�ȡ��ģ����
	//����ǰ��ֱ����������ʾ���ڣ���ʾ���ݻ��Զ�����
	for ( byteCount = 0; byteCount < fontLength; byteCount++ )
	{
			//һλһλ����Ҫ��ʾ����ɫ
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
  ILI9341_Clear(0,Line,LCD_X_LENGTH,(LCD_Currentfonts->Height));	/* ��������ʾȫ�� */

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

