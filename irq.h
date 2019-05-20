#ifndef __IRQ_H__
#define __IRQ_H__

#include <stdint.h>
#include <stdbool.h>

extern uint64_t sys_spurious_irqs7;
extern uint64_t sys_spurious_irqs15;

void remap_irqs(void);
bool is_spurious(uint16_t irq);

#endif // __IRQ_H__
