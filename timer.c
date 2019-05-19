#include <stdint.h>
#include "timer.h"
#include "io.h"

volatile uint64_t syscounter;

void init_timer(void)
{
	outb(0x43, 0x34);	// counter 0 mode 4
	outb(0x40, 0xA9);	// divider 4A9 (1193 gives 1.19318 MHz / 1193 = 1 kHz)
	outb(0x40, 0x4);
}
