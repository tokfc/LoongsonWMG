#include "TouchLCD.h"


void main()
{
	char getkey;
	int exitstate=0;
	LCDinit();
	DrawPage5();
	while(!exitstate)
	{
		getkey=GetPage5();
		if(getkey=='P')continue;
		switch(getkey){
			case '0':printf("0\n");continue;
			case '1':printf("1\n");continue;
			case '2':printf("2\n");continue;
			case '3':printf("3\n");continue;
			case '4':printf("4\n");continue;
			case '5':printf("5\n");continue;
			case '6':printf("6\n");continue;
			case '7':printf("7\n");continue;
			case '8':printf("8\n");continue;
			case '9':printf("9\n");continue;
			case 'Y':printf("是我\n");continue;
			case 'S':printf("退出中\n");exitstate=1;continue;
			default: continue;
		}
		
	}
	LCDRelease();
}