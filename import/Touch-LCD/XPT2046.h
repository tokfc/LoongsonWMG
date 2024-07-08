#ifndef  XPT2046_H
#define  XPT2046_H
#include "GPIO.h"
#include <stdio.h>
#include "ILI9341.h"

/******************************* 用到的GPIO预设   ********************************/
#define TOCSCL    0x0004000000000000UL
#define TOCCS     0x0008000000000000UL
#define TOCSDI    0x0010000000000000UL 
#define TOCSDO    0x0020000000000000UL
#define TOCIRQ    0x0040000000000000UL

#define TOC_CS_1 GPIOPinSet(TOCCS) 
#define TOC_CS_0 GPIOPinReset(TOCCS)

#define TOC_SCL_1 GPIOPinSet(TOCSCL)
#define TOC_SCL_0 GPIOPinReset(TOCSCL)

#define TOC_SDI_1 GPIOPinSet(TOCSDI)
#define TOC_SDI_0 GPIOPinReset(TOCSDI)

#define             XPT2046_PENIRQ_Read()             ((*GPIO_IN & TOCIRQ) == TOCIRQ)
#define             XPT2046_PENIRQ_ActiveLevel                     0

/******************************* XPT2046 触摸屏参数定义 ***************************/
//校准触摸屏时触摸坐标的AD值相差门限 
#define             XPT2046_THRESHOLD_CalDiff                    2               

#define	            XPT2046_CHANNEL_X 	                          0x90 	          //通道Y+的选择控制字	
#define	            XPT2046_CHANNEL_Y 	                          0xd0	          //通道X+的选择控制字

/******触摸状态机相关******/
typedef enum
{
	XPT2046_STATE_RELEASE  = 0,	//触摸释放
	XPT2046_STATE_WAITING,			//触摸按下
	XPT2046_STATE_PRESSED,			//触摸按下
}enumTouchState	;

#define TOUCH_PRESSED 				1
#define TOUCH_NOT_PRESSED			0

//触摸消抖阈值
#define DURIATION_TIME				2

typedef	struct          //液晶坐标结构体 
{		
   short x;			//记录最新的触摸参数值
   short y; 
} strType_XPT2046_Coordinate;


void XPT2046_Init( void );
void XPT2046_Write_Cmd(uint8 Cmd);


uint8 XPT2046_TouchDetect(void);
uint8 XPT2046_Get_TouchedPoint ( strType_XPT2046_Coordinate * pDisplayCoordinate);//带校准
uint8 XPT2046_ReadAdc_Smooth_XY ( strType_XPT2046_Coordinate * pScreenCoordinate );//无校准


#endif
