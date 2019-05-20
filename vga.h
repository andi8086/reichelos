#ifndef __VGA_H__
#define __VGA_H__

#define VIDEO 0xB8000

#define COLUMNS 80
#define LINES 25
#define ATTRIBUTE 7

#include <stdint.h>

void vga_init(unsigned char*);
void cursor(int x, int y);

#endif
