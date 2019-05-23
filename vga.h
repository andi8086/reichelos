#ifndef __VGA_H__
#define __VGA_H__

#include <stdint.h>

typedef enum {
	TEXT_80x25,
	TEXT_40x25,
	TEXT_90x30,
	TEXT_90x60,
	TEXT_40x50,
	TEXT_80x50
} VGA_TEXT_MODE;

#define VIDEO vga_screen_mem
#define LINES vga_text_lines
#define COLUMNS vga_text_cols
#define ATTRIBUTE vga_last_attrib

#define pokeb(seg, off, val) *((uint8_t *) (uint32_t)(seg << 4) + off) = val
#define pokew(seg, off, val) *((uint16_t *) (uint32_t)(seg << 4) + off) = val
#define peekb(seg, off) *((uint8_t *) (uint32_t)(seg << 4) + off)

extern uint8_t *vga_screen_mem;
extern uint8_t vga_text_lines;
extern uint8_t vga_text_cols;
extern uint8_t vga_last_attrib;

void vga_init();
void cursor(int x, int y);

#endif
