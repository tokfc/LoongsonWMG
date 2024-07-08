#ifndef GPIO_H
#define GPIO_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h> 

typedef unsigned long   uint64;
typedef unsigned int    uint32;
typedef unsigned short  uint16;
typedef unsigned char   uint8;




#define MAP_SIZE        0x10000
#define AUDIO_REG_BASE  0x1fe00000

#define OEN_REG	0x500UL
#define OUT_REG	0x510UL
#define IN_REG	0x520UL
#define INT_REG	0x530UL

#define GPIOPinSet(SetPin)         *GPIO_OUT=*GPIO_OUT|SetPin
#define GPIOPinReset(ResetPin)     *GPIO_OUT=*GPIO_OUT&(~ResetPin)
#define GPIOSegSet(SetValue)       *GPIO_OUT=*GPIO_OUT|SetValue
#define GPIOSegReset(ResetValue)   *GPIO_OUT=*GPIO_OUT&(~ResetValue)


volatile uint64* GPIO_OEN;
volatile uint64* GPIO_OUT;
volatile uint64* GPIO_IN;
volatile uint64* GPIO_INTEN;



static unsigned char *	map_base;
static int			dev_fd;

void GPIORegLoad(void);

void GPIORegRelease(void);

#endif

