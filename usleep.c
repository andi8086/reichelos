#include "usleep.h"
#include "timer.h"

void usleep(int32_t us)
{
	if (us < 100) us = 100;

	us = us / 100;
	kerneltimer = 0;	
	while (kerneltimer < us) asm volatile("nop");
}
