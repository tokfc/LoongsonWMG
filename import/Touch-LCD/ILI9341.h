#ifndef  ILI9341_H
#define  ILI9341_H
#include "GPIO.h"
#include <stdio.h>
#include "myFonts.h"

/******************************* 用到的GPIO预设   ********************************/
#define LCDCS     0x0000100000000000UL
#define LCDreset  0x0000200000000000UL
#define LCDRS     0x0000400000000000UL 
#define LCDSDI    0x0000800000000000UL
#define LCDSCL    0x0001000000000000UL
#define LCDSDO    0x0002000000000000UL

#define LCD_CS_1 GPIOPinSet(LCDCS) 
#define LCD_CS_0 GPIOPinReset(LCDCS)

#define LCD_RS_1 GPIOPinSet(LCDRS) 
#define LCD_RS_0 GPIOPinReset(LCDRS)

#define LCD_reset_1 GPIOPinSet(LCDreset) 
#define LCD_reset_0 GPIOPinReset(LCDreset)

#define LCD_SCL_1 GPIOPinSet(LCDSCL)
#define LCD_SCL_0 GPIOPinReset(LCDSCL)

#define LCD_SDI_1 GPIOPinSet(LCDSDI)
#define LCD_SDI_0 GPIOPinReset(LCDSDI)


/***************************** ILI934 显示区域的起始坐标和总行列数 ***************************/
#define      ILI9341_DispWindow_X_Star		    0     //起始点的X坐标
#define      ILI9341_DispWindow_Y_Star		    0     //起始点的Y坐标

#define 			ILI9341_LESS_PIXEL	  		240			//液晶屏较短方向的像素宽度
#define 			ILI9341_MORE_PIXEL	 		320			//液晶屏较长方向的像素宽度
/******************************* 定义 ILI934 显示屏常用颜色 ********************************/
#define      BACKGROUND		               0xF7B9   //默认背景颜色暖黄色

#define      WHITE		 		           0xFFFF	   //白色
#define      BLACK                         0x0000	   //黑色 
#define      GREY                          0xF7DE	   //灰色 
#define      BLUE                          0x001F	   //蓝色 
#define      BLUE2                         0x051F	   //浅蓝色 
#define      RED                           0xF800	   //红色       
#define      MAGENTA                       0xF81F	   //红紫色，洋红色 
#define      GREEN                         0x07E0	   //绿色 
#define      CYAN                          0x7FFF	   //蓝绿色，青色 
#define      YELLOW                        0xFFE0	   //黄色 
#define      BRED                          0xF81F
#define      GRED                          0xFFE0
#define      GBLUE                         0x07FF

#define      RGB24to16(RGB)				  ((RGB & 0xF80000)>>8)|(( RGB & 0xFC00 )>>5)|((RGB & 0xF8 )>>3)
/******************************* 定义 ILI9341 常用命令 ********************************/
#define      CMD_SetCoordinateX		 		    0x2A	     //设置X坐标
#define      CMD_SetCoordinateY		 		    0x2B	     //设置Y坐标
#define      CMD_SetPixel		 		          0x2C	     //填充像素

//根据液晶扫描方向而变化的XY像素宽度
//调用ILI9341_GramScan函数设置方向时会自动更改
extern uint16 LCD_X_LENGTH,LCD_Y_LENGTH; 

//液晶屏扫描模式
//参数可选值为0-7
extern uint8 LCD_SCAN_MODE;



uint64 LCD_DBconvertData(uint64 DataToConvert);

void                      ILI9341_Init                    ( void );
void                      ILI9341_Rst                     ( void );
void                      ILI9341_GramScan                ( uint8 ucOtion );

void 						ILI9341_Write_Cmd				(uint8 Cmd);
void 						ILI9341_Write_Data				(uint8 data);

void                      ILI9341_OpenWindow              ( uint16 usX, uint16 usY, uint16 usWidth, uint16 usHeight );
void                      ILI9341_Clear                   ( uint16 usX, uint16 usY, uint16 usWidth, uint16 usHeight );
void                      ILI9341_SetPointPixel           ( uint16 usX, uint16 usY );
void                      ILI9341_DrawLine                ( uint16 usX1, uint16 usY1, uint16 usX2, uint16 usY2 );
void                      ILI9341_DrawRectangle           ( uint16 usX_Start, uint16 usY_Start, uint16 usWidth, uint16 usHeight,uint8 ucFilled );

void                    ILI9341_DispChar_EN               ( uint16 usX, uint16 usY, const char cChar );
void                    ILI9341_DispStringLine_EN         ( uint16 line,char * pStr );
void                    ILI9341_DispString_EN             ( uint16 usX, uint16 usY, char * pStr );
void                    ILI9341_DispString_EN_YDir        ( uint16 usX, uint16 usY, char * pStr );


void 											LCD_SetFont											(sFONT *fonts);
sFONT 										    *LCD_GetFont										(void);
void 											LCD_ClearLine										(uint16 Line);
void 											LCD_SetBackColor								(uint16 Color);
void 											LCD_SetTextColor								(uint16 Color)	;
void 											LCD_SetColors										(uint16 TextColor, uint16 BackColor);
void 											LCD_GetColors										(uint16 *TextColor, uint16 *BackColor);

#endif
