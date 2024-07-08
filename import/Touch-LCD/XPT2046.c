#include "XPT2046.h"
#include <stdio.h>



void XPT2046_Delay (uint32 nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}


void XPT2046_Init()
{
  *GPIO_OEN=*GPIO_OEN&(~(TOCCS|TOCSCL|TOCSDI));       //控制线设置输出模式
  *GPIO_OEN=*GPIO_OEN|(TOCSDO|TOCIRQ);
  TOC_CS_1;//位选线闲时置1
  TOC_SCL_1;
  TOC_SDI_1;
}

void XPT2046_SPI_WR(uint8 cmd)
{
  uint8 counter1;
  //printf("Write SPI");
  for(counter1=0;counter1<8;counter1++)
  {
    if((cmd & (0x80>>counter1))==(0x80>>counter1)) {TOC_SDI_1;/*printf("1");*/}
    else {TOC_SDI_0;/*printf("0");*/}
	TOC_SCL_0;
    XPT2046_Delay(1);
    TOC_SCL_1;
    XPT2046_Delay(1);
  }
  //printf("b\n");
}

uint16 XPT2046_SPI_RD()
{
  uint16 tmp;
  uint8 counter1; 
  //printf("SPI Read 0b");
  for(counter1=0;counter1<16;counter1++)
  {
	tmp=tmp<<1;
    TOC_SCL_0;
    XPT2046_Delay(1);
	TOC_SCL_1;
    if((*GPIO_IN) & TOCSDO) {tmp|=0x01;/*printf("1");*/}
    else {tmp|=0x00;/*printf("0");*/}
    XPT2046_Delay(1);
  }
  //printf("\n");
  tmp=tmp>>4; //只有12位有效数据
  return tmp;
}


//读数据
uint16 XPT2046_Read_ADC(uint8 cmd)
{   
  uint16 tmp;

  TOC_CS_0;
  XPT2046_SPI_WR(cmd);
  //等待一个时钟的转换时间
  TOC_SCL_0;
  XPT2046_Delay(30);
  TOC_SCL_1;
  TOC_SCL_0;
  
  tmp=XPT2046_SPI_RD();  
  TOC_CS_1; 

  return tmp;  
}

/**自带校准
  * @brief  读取 XPT2046 的X通道和Y通道的AD值（12 bit，最大是4096）
  * @param  sX_Ad ：存放X通道AD值的地址
  * @param  sY_Ad ：存放Y通道AD值的地址
  * @retval 无
  */
void XPT2046_ReadAdc_XY ( short * sX_Ad, short * sY_Ad )  
{ 
	short sX_Ad_Temp, sY_Ad_Temp; 
	
	sX_Ad_Temp = XPT2046_Read_ADC ( XPT2046_CHANNEL_X );

	XPT2046_Delay ( 1 ); 
	sY_Ad_Temp = XPT2046_Read_ADC ( XPT2046_CHANNEL_Y ); 
	
	
	* sX_Ad = sX_Ad_Temp; 
	* sY_Ad = sY_Ad_Temp; 
	
	
}

uint8 XPT2046_ReadAdc_Smooth_XY ( strType_XPT2046_Coordinate * pScreenCoordinate )
{
	uint8 ucCount = 0, i;
	
	short sAD_X, sAD_Y;
	short sBufferArray [ 2 ] [ 10 ] = { { 0 },{ 0 } };  //坐标X和Y进行多次采样
	
	//存储采样中的最小值、最大值
	int lX_Min, lX_Max, lY_Min, lY_Max;


	/* 循环采样10次 */ 
	do					       				
	{		  
		XPT2046_ReadAdc_XY ( & sAD_X, & sAD_Y );  
		//printf("采样一次\n");
		sBufferArray [ 0 ] [ ucCount ] = sAD_X;  
		sBufferArray [ 1 ] [ ucCount ] = sAD_Y;
		
		ucCount ++;  
		
	}	while ( ( XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel ) && ( ucCount < 10 ) );//用户点击触摸屏时即TP_INT_IN信号为低 并且 ucCount<10
	
	

	
	/*如果成功采样10个样本，触屏不能采集10个数据则返回0*/
	if ( ucCount ==10 )		 					
	{
		
		lX_Max = lX_Min = sBufferArray [ 0 ] [ 0 ];
		lY_Max = lY_Min = sBufferArray [ 1 ] [ 0 ];       
		
		for ( i = 1; i < 10; i ++ )
		{
			if ( sBufferArray[ 0 ] [ i ] < lX_Min )
				lX_Min = sBufferArray [ 0 ] [ i ];
			
			else if ( sBufferArray [ 0 ] [ i ] > lX_Max )
				lX_Max = sBufferArray [ 0 ] [ i ];

		}
		
		for ( i = 1; i < 10; i ++ )
		{
			if ( sBufferArray [ 1 ] [ i ] < lY_Min )
				lY_Min = sBufferArray [ 1 ] [ i ];
			
			else if ( sBufferArray [ 1 ] [ i ] > lY_Max )
				lY_Max = sBufferArray [ 1 ] [ i ];

		}
		
		
		
		/*去除最小值和最大值之后求平均值*/
		pScreenCoordinate ->x =  ( sBufferArray [ 0 ] [ 0 ] + sBufferArray [ 0 ] [ 1 ] + sBufferArray [ 0 ] [ 2 ] + sBufferArray [ 0 ] [ 3 ] + sBufferArray [ 0 ] [ 4 ] + 
		                           sBufferArray [ 0 ] [ 5 ] + sBufferArray [ 0 ] [ 6 ] + sBufferArray [ 0 ] [ 7 ] + sBufferArray [ 0 ] [ 8 ] + sBufferArray [ 0 ] [ 9 ] - lX_Min-lX_Max ) >> 3;
		
		pScreenCoordinate ->y =  ( sBufferArray [ 1 ] [ 0 ] + sBufferArray [ 1 ] [ 1 ] + sBufferArray [ 1 ] [ 2 ] + sBufferArray [ 1 ] [ 3 ] + sBufferArray [ 1 ] [ 4 ] + 
		                           sBufferArray [ 1 ] [ 5 ] + sBufferArray [ 1 ] [ 6 ] + sBufferArray [ 1 ] [ 7 ] + sBufferArray [ 1 ] [ 8 ] + sBufferArray [ 1 ] [ 9 ] - lY_Min-lY_Max ) >> 3; 
		
		//加上校准
		pScreenCoordinate ->x = 1.16087  * (0.078125   * pScreenCoordinate ->x) - 29.56267;
		pScreenCoordinate ->y = 1.1833   * (0.05859375 * pScreenCoordinate ->y) - 21.2994;
		
		
		//printf("成功get x =%d ;  y= %d .\n",(pScreenCoordinate ->x),(pScreenCoordinate ->y));
		return 1;		
		
	} 	
	return 0;    	
}

void ILI9341_DrawCross ( uint16 usX, uint16 usY )
{
	ILI9341_DrawLine(usX-10,usY,usX+10,usY);
	ILI9341_DrawLine(usX, usY - 10, usX, usY+10);	
}



/**
  * @brief  触摸屏检测状态机，直接循环调用，带有消抖，和持续更新中断寄存器
  * @retval 触摸状态
	*   该返回值为以下值之一：
  *     @arg TOUCH_PRESSED :触摸按下
  *     @arg TOUCH_NOT_PRESSED :无触摸
  */
uint8 XPT2046_TouchDetect(void)
{ 
	static enumTouchState touch_state = XPT2046_STATE_RELEASE;
	static uint32 i;
	uint8 detectResult = TOUCH_NOT_PRESSED;
	
	switch(touch_state)
	{
		case XPT2046_STATE_RELEASE:	
			if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel) //第一次出现触摸信号
			{
				touch_state = XPT2046_STATE_WAITING;
				detectResult =TOUCH_NOT_PRESSED;
				}
			else	//无触摸
			{
				touch_state = XPT2046_STATE_RELEASE;
				detectResult =TOUCH_NOT_PRESSED;
			}
			break;
				
		case XPT2046_STATE_WAITING:
				if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel)
				{
					 i++;
					//等待时间大于阈值则认为触摸被按下
					//消抖时间 = DURIATION_TIME * 本函数被调用的时间间隔
					//如在定时器中调用，每10ms调用一次，则消抖时间为：DURIATION_TIME*10ms
					if(i > DURIATION_TIME)		
					{
						i=0;
						touch_state = XPT2046_STATE_PRESSED;
						detectResult = TOUCH_PRESSED;
					}
					else												//等待时间累加
					{
						touch_state = XPT2046_STATE_WAITING;
						detectResult =	 TOUCH_NOT_PRESSED;					
					}
				}
				else	//等待时间值未达到阈值就为无效电平，当成抖动处理					
				{
				    i = 0;
            touch_state = XPT2046_STATE_RELEASE; 
						detectResult = TOUCH_NOT_PRESSED;
				}
		
			break;
		
		case XPT2046_STATE_PRESSED:	
				if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel)		//触摸持续按下
				{
					touch_state = XPT2046_STATE_PRESSED;
					detectResult = TOUCH_PRESSED;
				}
				else	//触摸释放
				{
					touch_state = XPT2046_STATE_RELEASE;
					detectResult = TOUCH_NOT_PRESSED;
				}
			break;			
		
		default:
				touch_state = XPT2046_STATE_RELEASE;
				detectResult = TOUCH_NOT_PRESSED;
				break;
	
	}		
	
	return detectResult;
}


