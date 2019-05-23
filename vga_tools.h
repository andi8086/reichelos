#ifndef __VGA_TOOLS_H__
#define __VGA_TOOLS_H__

extern void dump(unsigned char *regs, unsigned count);
extern void dump_regs(unsigned char *regs);
extern void read_regs(unsigned char *regs);
extern void write_regs(unsigned char *regs);
extern void set_plane(unsigned p);
extern unsigned get_fb_seg(void);
extern void vmemwr(unsigned dst_off, unsigned char *src, unsigned count);
extern void vpokeb(unsigned off, unsigned val);
extern unsigned vpeekb(unsigned off);
extern void write_font(unsigned char *buf, unsigned font_height);

extern void (*g_write_pixel)(unsigned x, unsigned y, unsigned c);
extern unsigned g_wd, g_ht;

extern void write_pixel1(unsigned x, unsigned y, unsigned c);

extern void write_pixel2(unsigned x, unsigned y, unsigned c);
extern void write_pixel4p(unsigned x, unsigned y, unsigned c);
extern void write_pixel8(unsigned x, unsigned y, unsigned c);

extern void write_pixel8x(unsigned x, unsigned y, unsigned c);

extern void draw_x(void);
extern void dump_state(void);

extern void set_text_mode(int hi_res);

extern void demo_graphics(void);
extern unsigned char reverse_bits(unsigned char arg);
extern void font512(void);

#endif // __VGA_TOOLS_H__
