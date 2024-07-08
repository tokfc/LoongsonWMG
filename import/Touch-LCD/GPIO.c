#include "GPIO.h"

void GPIORegLoad()
{

	dev_fd = open("/dev/mem", O_RDWR | O_SYNC);      

	if (dev_fd < 0)  
	{
		printf("open(/dev/mem) failed.");    
		return ;
	}  

	map_base=(unsigned char * )mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dev_fd, AUDIO_REG_BASE );
	if(map_base == (void *)-1) {printf("map create failed!\n");exit(1);}
	printf("map create succeed!\n");

	GPIO_OEN	=(volatile uint64*)(map_base+OEN_REG);
	GPIO_OUT	=(volatile uint64*)(map_base+OUT_REG);
	GPIO_IN		=(volatile uint64*)(map_base+IN_REG);
	GPIO_INTEN=(volatile uint64*)(map_base+INT_REG);
	printf("Register Ready!\n");

}

void GPIORegRelease()
{
	munmap(map_base,MAP_SIZE);//解除映射关系

	if(dev_fd) close(dev_fd);
}

