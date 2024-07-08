#ifndef TouchLCD_H_
#define TouchLCD_H_

#include "ILI9341.h"
#include "XPT2046.h"
#include "GPIO.h"

void LCDinit(void);
void DrawPage1(void); //大标题
void DrawPage2(void); //取件方式
void DrawPage3(void); //查柜信息
void DrawPage4(void); //扫脸取件
void DrawPage5(void); //存件，请输入货主代号
void DrawPage6(void); //显示二维码

char GetPage1(void);
char GetPage2(void);
char GetPage3(void);
char GetPage4(void);
char GetPage5(void);
char GetPage6(void);


void LCDDispStringLine_EN( uint16 line,char * pStr );
void LCDRelease(void);

#endif