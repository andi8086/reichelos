#include "io.h"
#include "irq.h"
#include <stdbool.h>

#define PIC1_CMD                    0x20
#define PIC1_DATA                   0x21
#define PIC2_CMD                    0xA0
#define PIC2_DATA                   0xA1
#define PIC_READ_IRR                0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR                0x0b    /* OCW3 irq service next CMD read */

#define IRQ0_VEC		    0x20
#define IRQ8_VEC		    0x28

uint64_t sys_spurious_irqs7;
uint64_t sys_spurious_irqs15;

uint16_t readISR(void)
{
	outb(PIC1_CMD, PIC_READ_ISR);
	outb(PIC2_CMD, PIC_READ_ISR);
	return (uint16_t) inb(PIC2_CMD) << 8 | inb(PIC1_CMD);
}

bool is_spurious(uint16_t irq)
{
	return !(readISR() & (1 << irq));
}

void remap_irqs(void)
{
	outb(PIC1_CMD, 0x11);
	outb(PIC2_CMD, 0x11);
	outb(PIC1_DATA, IRQ0_VEC);
	outb(PIC2_DATA, IRQ8_VEC);
	outb(PIC1_DATA, 0x04);
	outb(PIC2_DATA, 0x02);
	outb(PIC1_DATA, 0x01);
	outb(PIC2_DATA, 0x01);
	outb(PIC1_DATA, 0x0);
	outb(PIC2_DATA, 0x0);
}
