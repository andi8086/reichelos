#include "vga.h"

uint8_t *vga_screen_mem;
uint8_t vga_text_lines;
uint8_t vga_text_cols;
uint8_t vga_last_attrib;

void vga_init()
{
	vga_screen_mem = (uint8_t *)0xB8000;
	set_text_mode(0);
}
