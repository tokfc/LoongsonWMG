#ifndef  XPT2046_H
#define  XPT2046_H
#include "GPIO.h"
#include <stdio.h>
#include "ILI9341.h"

/******************************* �õ���GPIOԤ��   ********************************/
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

/******************************* XPT2046 �������������� ***************************/
//У׼������ʱ���������ADֵ������� 
#define             XPT2046_THRESHOLD_CalDiff                    2               

#define	            XPT2046_CHANNEL_X 	                          0x90 	          //ͨ��Y+��ѡ�������	
#define	            XPT2046_CHANNEL_Y 	                          0xd0	          //ͨ��X+��ѡ�������

/******����״̬�����******/
typedef enum
{
	XPT2046_STATE_RELEASE  = 0,	//�����ͷ�
	XPT2046_STATE_WAITING,			//��������
	XPT2046_STATE_PRESSED,			//��������
}enumTouchState	;

#define TOUCH_PRESSED 				1
#define TOUCH_NOT_PRESSED			0

//����������ֵ
#define DURIATION_TIME				2

typedef	struct          //Һ������ṹ�� 
{		
   short x;			//��¼���µĴ�������ֵ
   short y; 
} strType_XPT2046_Coordinate;


void XPT2046_Init( void );
void XPT2046_Write_Cmd(uint8 Cmd);


uint8 XPT2046_TouchDetect(void);
uint8 XPT2046_Get_TouchedPoint ( strType_XPT2046_Coordinate * pDisplayCoordinate);//��У׼
uint8 XPT2046_ReadAdc_Smooth_XY ( strType_XPT2046_Coordinate * pScreenCoordinate );//��У׼


#endif
