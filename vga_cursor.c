#include "vga.h"
#include "io.h"

void cursor(int x, int y)
{
	uint16_t pos = (y*COLUMNS)+x;
	outb(0x3D4, 14);
	outb(0x3D5, (pos >> 8) & 0xFF);
	outb(0x3D4, 15);
	outb(0x3D5, pos & 0xFF);
}
