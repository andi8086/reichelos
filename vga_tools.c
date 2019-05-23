#include "vga.h"
#include "vga_regs.h"
#include "vga_fonts.h"
#include "vga_modes.h"
#include "vga_tools.h"
#include "io.h"

#define puts(c) ;
#define getch(c) ;
#define printf(s,...) ;

void dump(unsigned char *regs, unsigned count)
{
	unsigned i;

	i = 0;
	printf("\t");
	for(; count != 0; count--)
	{
		printf("0x%02X,", *regs);
		i++;
		if(i >= 8)
		{
			i = 0;
			printf("\n\t");
		}
		else
			printf(" ");
		regs++;
	}
	printf("\n");
}

void dump_regs(unsigned char *regs)
{
	printf("unsigned char g_mode[] =\n");
	printf("{\n");
/* dump MISCELLANEOUS reg */
	printf("/* MISC */\n");
	printf("\t0x%02X,\n", *regs);
	regs++;
/* dump SEQUENCER regs */
	printf("/* SEQ */\n");
	dump(regs, VGA_NUM_SEQ_REGS);
	regs += VGA_NUM_SEQ_REGS;
/* dump CRTC regs */
	printf("/* CRTC */\n");
	dump(regs, VGA_NUM_CRTC_REGS);
	regs += VGA_NUM_CRTC_REGS;
/* dump GRAPHICS CONTROLLER regs */
	printf("/* GC */\n");
	dump(regs, VGA_NUM_GC_REGS);
	regs += VGA_NUM_GC_REGS;
/* dump ATTRIBUTE CONTROLLER regs */
	printf("/* AC */\n");
	dump(regs, VGA_NUM_AC_REGS);
	regs += VGA_NUM_AC_REGS;
	printf("};\n");
}

void read_regs(unsigned char *regs)
{
	unsigned i;

/* read MISCELLANEOUS reg */
	*regs = inb(VGA_MISC_READ);
	regs++;
/* read SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
		outb(VGA_SEQ_INDEX, i);
		*regs = inb(VGA_SEQ_DATA);
		regs++;
	}
/* read CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		outb(VGA_CRTC_INDEX, i);
		*regs = inb(VGA_CRTC_DATA);
		regs++;
	}
/* read GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++)
	{
		outb(VGA_GC_INDEX, i);
		*regs = inb(VGA_GC_DATA);
		regs++;
	}
/* read ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++)
	{
		(void)inb(VGA_INSTAT_READ);
		outb(VGA_AC_INDEX, i);
		*regs = inb(VGA_AC_READ);
		regs++;
	}
/* lock 16-color palette and unblank display */
	(void)inb(VGA_INSTAT_READ);
	outb(VGA_AC_INDEX, 0x20);
}

void write_regs(unsigned char *regs)
{
	unsigned i;

/* write MISCELLANEOUS reg */
	outb(VGA_MISC_WRITE, *regs);
	regs++;
/* write SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
		outb(VGA_SEQ_INDEX, i);
		outb(VGA_SEQ_DATA, *regs);
		regs++;
	}
/* unlock CRTC registers */
	outb(VGA_CRTC_INDEX, 0x03);
	outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) | 0x80);
	outb(VGA_CRTC_INDEX, 0x11);
	outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) & ~0x80);
/* make sure they remain unlocked */
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;
/* write CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		outb(VGA_CRTC_INDEX, i);
		outb(VGA_CRTC_DATA, *regs);
		regs++;
	}
/* write GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++)
	{
		outb(VGA_GC_INDEX, i);
		outb(VGA_GC_DATA, *regs);
		regs++;
	}
/* write ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++)
	{
		(void)inb(VGA_INSTAT_READ);
		outb(VGA_AC_INDEX, i);
		outb(VGA_AC_WRITE, *regs);
		regs++;
	}
/* lock 16-color palette and unblank display */
	(void)inb(VGA_INSTAT_READ);
	outb(VGA_AC_INDEX, 0x20);
}

 void set_plane(unsigned p)
{
	unsigned char pmask;

	p &= 3;
	pmask = 1 << p;
/* set read plane */
	outb(VGA_GC_INDEX, 4);
	outb(VGA_GC_DATA, p);
/* set write plane */
	outb(VGA_SEQ_INDEX, 2);
	outb(VGA_SEQ_DATA, pmask);
}

 unsigned get_fb_seg(void)
{
	unsigned seg;

	outb(VGA_GC_INDEX, 6);
	seg = inb(VGA_GC_DATA);
	seg >>= 2;
	seg &= 3;
	switch(seg)
	{
	case 0:
	case 1:
		seg = 0xA000;
		break;
	case 2:
		seg = 0xB000;
		break;
	case 3:
		seg = 0xB800;
		break;
	}
	return seg;
}

 void vmemwr(unsigned dst_off, unsigned char *src, unsigned count)
{
	while(count--) {
		*(VIDEO + dst_off) = *src;
		src++;
		dst_off++;
	}
}

void vpokeb(unsigned off, unsigned val)
{
	pokeb(get_fb_seg(), off, val);
}

 unsigned vpeekb(unsigned off)
{
	return peekb(get_fb_seg(), off);
}

 void write_font(unsigned char *buf, unsigned font_height)
{
	unsigned char seq2, seq4, gc4, gc5, gc6;
	unsigned i;

/* save registers
set_plane() modifies GC 4 and SEQ 2, so save them as well */
	outb(VGA_SEQ_INDEX, 2);
	seq2 = inb(VGA_SEQ_DATA);

	outb(VGA_SEQ_INDEX, 4);
	seq4 = inb(VGA_SEQ_DATA);
/* turn off even-odd addressing (set flat addressing)
assume: chain-4 addressing already off */
	outb(VGA_SEQ_DATA, seq4 | 0x04);

	outb(VGA_GC_INDEX, 4);
	gc4 = inb(VGA_GC_DATA);

	outb(VGA_GC_INDEX, 5);
	gc5 = inb(VGA_GC_DATA);
/* turn off even-odd addressing */
	outb(VGA_GC_DATA, gc5 & ~0x10);

	outb(VGA_GC_INDEX, 6);
	gc6 = inb(VGA_GC_DATA);
/* turn off even-odd addressing */
	outb(VGA_GC_DATA, gc6 & ~0x02);
/* write font to plane P4 */
	set_plane(2);
/* write font 0 */
	for(i = 0; i < 256; i++)
	{
		vmemwr(16384u * 0 + i * 32, buf, font_height);
		buf += font_height;
	}
#if 0
/* write font 1 */
	for(i = 0; i < 256; i++)
	{
		vmemwr(16384u * 1 + i * 32, buf, font_height);
		buf += font_height;
	}
#endif
/* restore registers */
	outb(VGA_SEQ_INDEX, 2);
	outb(VGA_SEQ_DATA, seq2);
	outb(VGA_SEQ_INDEX, 4);
	outb(VGA_SEQ_DATA, seq4);
	outb(VGA_GC_INDEX, 4);
	outb(VGA_GC_DATA, gc4);
	outb(VGA_GC_INDEX, 5);
	outb(VGA_GC_DATA, gc5);
	outb(VGA_GC_INDEX, 6);
	outb(VGA_GC_DATA, gc6);
}

 void (*g_write_pixel)(unsigned x, unsigned y, unsigned c);
 unsigned g_wd, g_ht;

 void write_pixel1(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes;
	unsigned off, mask;

	c = (c & 1) * 0xFF;
	wd_in_bytes = g_wd / 8;
	off = wd_in_bytes * y + x / 8;
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	vpokeb(off, (vpeekb(off) & ~mask) | (c & mask));
}

 void write_pixel2(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes, off, mask;

	c = (c & 3) * 0x55;
	wd_in_bytes = g_wd / 4;
	off = wd_in_bytes * y + x / 4;
	x = (x & 3) * 2;
	mask = 0xC0 >> x;
	vpokeb(off, (vpeekb(off) & ~mask) | (c & mask));
}

 void write_pixel4p(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes, off, mask, p, pmask;

	wd_in_bytes = g_wd / 8;
	off = wd_in_bytes * y + x / 8;
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	pmask = 1;
	for(p = 0; p < 4; p++)
	{
		set_plane(p);
		if(pmask & c)
			vpokeb(off, vpeekb(off) | mask);
		else
			vpokeb(off, vpeekb(off) & ~mask);
		pmask <<= 1;
	}
}

 void write_pixel8(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes;
	unsigned off;

	wd_in_bytes = g_wd;
	off = wd_in_bytes * y + x;
	vpokeb(off, c);
}

 void write_pixel8x(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes;
	unsigned off;

	wd_in_bytes = g_wd / 4;
	off = wd_in_bytes * y + x / 4;
	set_plane(x & 3);
	vpokeb(off, c);
}

 void draw_x(void)
{
	unsigned x, y;

/* clear screen */
	for(y = 0; y < g_ht; y++)
		for(x = 0; x < g_wd; x++)
			g_write_pixel(x, y, 0);
/* draw 2-color X */
	for(y = 0; y < g_ht; y++)
	{
		g_write_pixel((g_wd - g_ht) / 2 + y, y, 1);
		g_write_pixel((g_ht + g_wd) / 2 - y, y, 2);
	}
	getch();
}

void dump_state(void)
{
	unsigned char state[VGA_NUM_REGS];

	read_regs(state);
	dump_regs(state);
}

void set_text_mode(VGA_TEXT_MODE mode)
{
	unsigned rows, cols, ht, i;

	switch(mode) {
	case TEXT_40x25:
		write_regs(g_40x25_text);
		cols = 90;
		rows = 60;
		ht = 16;
		break;
	case TEXT_40x50:
		write_regs(g_40x50_text);
		cols = 40;
		rows = 50;
		ht = 8;
		break;
	case TEXT_80x50:
		write_regs(g_80x50_text);
		cols = 80;
		rows = 50;
		ht = 8;
		break;
		break;
	case TEXT_90x30:
		write_regs(g_90x30_text);
		cols = 90;
		rows = 30;
		ht = 16;
		break;
	case TEXT_90x60:
		write_regs(g_90x60_text);
		cols = 90;
		rows = 60;
		ht = 8;
		break;
	case TEXT_80x25:
	default:
		write_regs(g_80x25_text);
		cols = 80;
		rows = 25;
		ht = 16;
	}
/* set font */
	if(ht >= 16)
		write_font(g_8x16_font, 16);
	else
		write_font(g_8x8_font, 8);
/* tell the BIOS what we've done, so BIOS text output works OK */
	pokew(0x40, 0x4A, cols);	/* columns on screen */
	pokew(0x40, 0x4C, cols * rows * 2); /* framebuffer size */
	pokew(0x40, 0x50, 0);		/* cursor pos'n */
	pokeb(0x40, 0x60, ht - 1);	/* cursor shape */
	pokeb(0x40, 0x61, ht - 2);
	pokeb(0x40, 0x84, rows - 1);	/* rows on screen - 1 */
	pokeb(0x40, 0x85, ht);		/* char height */
/* set white-on-black attributes for all text */
	for(i = 0; i < cols * rows; i++)
		pokeb(0xB800, i * 2 + 1, 7);


	COLUMNS = cols;
	LINES = rows;
	ATTRIBUTE = 0x07;
}

void demo_graphics(void)
{
	printf("Screen-clear in 16-color mode will be VERY SLOW\n"
		"Press a key to continue\n");
	getch();
/* 4-color */
	write_regs(g_320x200x4);
	g_wd = 320;
	g_ht = 200;
	g_write_pixel = write_pixel2;
	draw_x();
/* 16-color */
	write_regs(g_640x480x16);
	g_wd = 640;
	g_ht = 480;
	g_write_pixel = write_pixel4p;
	draw_x();
/* 256-color */
	write_regs(g_320x200x256);
	g_wd = 320;
	g_ht = 200;
	g_write_pixel = write_pixel8;
	draw_x();
/* 256-color Mode-X */
	write_regs(g_320x200x256_modex);
	g_wd = 320;
	g_ht = 200;
	g_write_pixel = write_pixel8x;
	draw_x();
/* go back to 80x25 text mode */
	set_text_mode(0);
}

 unsigned char reverse_bits(unsigned char arg)
{
	unsigned char ret_val = 0;

	if(arg & 0x01)
		ret_val |= 0x80;
	if(arg & 0x02)
		ret_val |= 0x40;
	if(arg & 0x04)
		ret_val |= 0x20;
	if(arg & 0x08)
		ret_val |= 0x10;
	if(arg & 0x10)
		ret_val |= 0x08;
	if(arg & 0x20)
		ret_val |= 0x04;
	if(arg & 0x40)
		ret_val |= 0x02;
	if(arg & 0x80)
		ret_val |= 0x01;
	return ret_val;
}

 void font512(void)
{
/* Turbo C++ 1.0 seems to 'lose' any data declared ' const' */
	/*static*/ const char msg1[] = "!txet sdrawkcaB";
	/*static*/ const char msg2[] = "?rorrim a toG";
/**/
	unsigned char seq2, seq4, gc4, gc5, gc6;
	unsigned font_height, i, j;

/* start in 80x25 text mode */
	set_text_mode(0);
/* code pasted in from write_font():
save registers
set_plane() modifies GC 4 and SEQ 2, so save them as well */
	outb(VGA_SEQ_INDEX, 2);
	seq2 = inb(VGA_SEQ_DATA);

	outb(VGA_SEQ_INDEX, 4);
	seq4 = inb(VGA_SEQ_DATA);
/* turn off even-odd addressing (set flat addressing)
assume: chain-4 addressing already off */
	outb(VGA_SEQ_DATA, seq4 | 0x04);

	outb(VGA_GC_INDEX, 4);
	gc4 = inb(VGA_GC_DATA);

	outb(VGA_GC_INDEX, 5);
	gc5 = inb(VGA_GC_DATA);
/* turn off even-odd addressing */
	outb(VGA_GC_DATA, gc5 & ~0x10);

	outb(VGA_GC_INDEX, 6);
	gc6 = inb(VGA_GC_DATA);
/* turn off even-odd addressing */
	outb(VGA_GC_DATA, gc6 & ~0x02);
/* write font to plane P4 */
	set_plane(2);
/* this is different from write_font():
use font 1 instead of font 0, and use it for BACKWARD text */
	font_height = 16;
	for(i = 0; i < 256; i++)
	{
		for(j = 0; j < font_height; j++)
		{
			vpokeb(16384u * 1 + 32 * i + j,
				reverse_bits(
					g_8x16_font[font_height * i + j]));
		}
	}
/* restore registers */
	outb(VGA_SEQ_INDEX, 2);
	outb(VGA_SEQ_DATA, seq2);
	outb(VGA_SEQ_INDEX, 4);
	outb(VGA_SEQ_DATA, seq4);
	outb(VGA_GC_INDEX, 4);
	outb(VGA_GC_DATA, gc4);
	outb(VGA_GC_INDEX, 5);
	outb(VGA_GC_DATA, gc5);
	outb(VGA_GC_INDEX, 6);
	outb(VGA_GC_DATA, gc6);
/* now: sacrifice attribute bit b3 (foreground intense color)
use it to select characters 256-511 in the second font */
	outb(VGA_SEQ_INDEX, 3);
	outb(VGA_SEQ_DATA, 4);
/* xxx - maybe re-program 16-color palette here
so attribute bit b3 is no longer used for 'intense' */
	for(i = 0; i < sizeof(msg1); i++)
	{
		vpokeb((80 * 8  + 40 + i) * 2 + 0, msg1[i]);
/* set attribute bit b3 for backward font */
		vpokeb((80 * 8  + 40 + i) * 2 + 1, 0x0F);
	}
	for(i = 0; i < sizeof(msg2); i++)
	{
		vpokeb((80 * 16 + 40 + i) * 2 + 0, msg2[i]);
		vpokeb((80 * 16 + 40 + i) * 2 + 1, 0x0F);
	}
}
