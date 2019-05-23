#include "cmos.h"
#include "io.h"

uint8_t read_cmos(uint8_t index)
{
	outb(0x70, index);
	return inb(0x71);
}
