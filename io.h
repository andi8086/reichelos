#ifndef __IO_H__
#define __IO_H__

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val)
{
	asm volatile("outb %0, %1" : : "dN"(port), "a"(val));
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile("inb %0, %1" : "=a"(ret) : "Nd"(port));
	return ret;
}

#endif // __IO_H__
