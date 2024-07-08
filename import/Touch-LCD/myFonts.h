#ifndef __MYFONTs_H
#define __MYFONTs_H       

typedef unsigned long   uint64;
typedef unsigned int    uint32;
typedef unsigned short  uint16;
typedef unsigned char   uint8;

/** @defgroup FONTS_Exported_Types
  * @{
  */ 
typedef struct _tFont
{    
  const uint8 *table;
  uint16 Width;
  uint16 Height;
  
} sFONT;

extern sFONT FontEN8x16;
extern sFONT BIGTitle;
extern sFONT Tips1;
extern sFONT Tips2;
extern sFONT Tips3;
extern sFONT Button1;
extern sFONT Button2;
extern sFONT Button3;
extern sFONT Button4;
extern sFONT Button5;
extern sFONT Button6;
extern sFONT LoongsonQRcode;


#define LINE(x) ((x) * (((sFONT *)LCD_GetFont())->Height))

#endif
