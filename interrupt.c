#include "irq.h"
#include "interrupt.h"
#include "mem.h"
#include "timer.h"
#include "io.h"

#pragma(__align(32))
struct __attribute__((packed)) IDT idt[48];

struct __attribute__((packed)) IDTDESC {
	uint16_t limit;
	uint32_t base;
} idtdesc;

void init_interrupt(struct IDT *desc, uint16_t selector, uint32_t offset,
		    uint8_t type, uint8_t dpl)
{
	desc->offset_lo = offset & 0xFFFF;
	desc->offset_mid = (offset >> 16) & 0xFFFF;
	desc->ist = 0;
	desc->selector = selector;
	desc->dpl = dpl;
	desc->type = type;
	desc->present = 1;
} 

void init_interrupts(void)
{
	init_interrupt(&idt[0], 0x08, (uint32_t) isr0, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[1], 0x08, (uint32_t) isr1, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[2], 0x08, (uint32_t) isr2, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[3], 0x08, (uint32_t) isr3, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[4], 0x08, (uint32_t) isr4, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[5], 0x08, (uint32_t) isr5, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[6], 0x08, (uint32_t) isr6, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[7], 0x08, (uint32_t) isr7, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[8], 0x08, (uint32_t) isr8, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[9], 0x08, (uint32_t) isr9, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[10], 0x08, (uint32_t) isr10, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[11], 0x08, (uint32_t) isr11, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[12], 0x08, (uint32_t) isr12, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[13], 0x08, (uint32_t) isr13, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[14], 0x08, (uint32_t) isr14, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[15], 0x08, (uint32_t) isr15, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[16], 0x08, (uint32_t) isr16, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[17], 0x08, (uint32_t) isr17, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[18], 0x08, (uint32_t) isr18, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[19], 0x08, (uint32_t) isr19, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[20], 0x08, (uint32_t) isr20, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[21], 0x08, (uint32_t) isr21, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[22], 0x08, (uint32_t) isr22, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[23], 0x08, (uint32_t) isr23, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[24], 0x08, (uint32_t) isr24, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[25], 0x08, (uint32_t) isr25, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[26], 0x08, (uint32_t) isr26, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[27], 0x08, (uint32_t) isr27, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[28], 0x08, (uint32_t) isr28, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[29], 0x08, (uint32_t) isr29, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[30], 0x08, (uint32_t) isr30, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[31], 0x08, (uint32_t) isr31, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[32], 0x08, (uint32_t) isr32, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[33], 0x08, (uint32_t) isr33, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[34], 0x08, (uint32_t) isr34, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[35], 0x08, (uint32_t) isr35, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[36], 0x08, (uint32_t) isr36, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[37], 0x08, (uint32_t) isr37, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[38], 0x08, (uint32_t) isr38, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[39], 0x08, (uint32_t) isr39, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[40], 0x08, (uint32_t) isr40, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[41], 0x08, (uint32_t) isr41, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[42], 0x08, (uint32_t) isr42, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[43], 0x08, (uint32_t) isr43, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[44], 0x08, (uint32_t) isr44, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[45], 0x08, (uint32_t) isr45, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[46], 0x08, (uint32_t) isr46, DESCTYPE_INT_GATE_386, 0); 
	init_interrupt(&idt[47], 0x08, (uint32_t) isr47, DESCTYPE_INT_GATE_386, 0); 

	idtdesc.limit = 48*8 - 1;
	idtdesc.base = (uint32_t) &idt[0];

	asm volatile ("lidt [eax]"::"a"(&idtdesc));

	remap_irqs();
}

void ISRCODE isr0(struct interrupt_frame *f) // div by zero
{
	asm volatile("nop");
}

void ISRCODE isr1(struct interrupt_frame* f) // debug
{
	asm volatile("nop");
}

void ISRCODE isr2(struct interrupt_frame* f) // NMI
{
	asm volatile("nop");
}

void ISRCODE isr3(struct interrupt_frame* f) // breakpoint
{
	asm volatile("nop");
}

void ISRCODE isr4(struct interrupt_frame* f) // overflow
{
	asm volatile("nop");
}

void ISRCODE isr5(struct interrupt_frame* f) // bound range exceed
{
	asm volatile("nop");
}

void ISRCODE isr6(struct interrupt_frame* f) // invalid opcode
{
	asm volatile("nop");
}

void ISRCODE isr7(struct interrupt_frame* f) // device not available
{
	asm volatile("nop");
}

void ISRCODE isr8(struct interrupt_frame* f, uint32_t err) // double fault
{
	asm volatile("nop");
}

void ISRCODE isr9(struct interrupt_frame* f) // coprocessor seg overrun
{
	asm volatile("nop");
}

void ISRCODE isr10(struct interrupt_frame* f, uint32_t err) // invalid tss
{
	asm volatile("nop");
}

void ISRCODE isr11(struct interrupt_frame* f, uint32_t err) // seg not present
{
	asm volatile("nop");
}

void ISRCODE isr12(struct interrupt_frame* f, uint32_t err) // stack-seg fault
{
	asm volatile("nop");
}

void ISRCODE isr13(struct interrupt_frame* f, uint32_t err) // gp
{
	asm volatile("nop");
}

void ISRCODE isr14(struct interrupt_frame* f, uint32_t err) // page fault
{
	asm volatile("nop");
}

void ISRCODE isr15(struct interrupt_frame* f) // reserved
{
	asm volatile("nop");
}

void ISRCODE isr16(struct interrupt_frame* f) // x87 fpu exception 
{
	asm volatile("nop");
}

void ISRCODE isr17(struct interrupt_frame* f, uint32_t err) // alignment check
{
	asm volatile("nop");
}

void ISRCODE isr18(struct interrupt_frame* f) // machine check
{
	asm volatile("nop");
}

void ISRCODE isr19(struct interrupt_frame* f) // SIMD FPU exception
{
	asm volatile("nop");
}

void ISRCODE isr20(struct interrupt_frame* f) // virtualiation exception
{
}

void ISRCODE isr21(struct interrupt_frame* f) // reserved
{
}

void ISRCODE isr22(struct interrupt_frame* f) // reserved
{
}

void ISRCODE isr23(struct interrupt_frame* f) // reserved
{
}

void ISRCODE isr24(struct interrupt_frame* f) // reserved
{
}

void ISRCODE isr25(struct interrupt_frame* f) // reserved
{
}

void ISRCODE isr26(struct interrupt_frame* f) // reserved
{
}

void ISRCODE isr27(struct interrupt_frame* f) // reserved
{
}

void ISRCODE isr28(struct interrupt_frame* f) // reserved
{
}

void ISRCODE isr29(struct interrupt_frame* f) // reserved
{
}

void ISRCODE isr30(struct interrupt_frame* f, uint32_t err) // security exception
{
}

void ISRCODE isr31(struct interrupt_frame* f) // reserved
{
}

void ISRCODE isr32(struct interrupt_frame* f) // IRQ0
{
	syscounter++;
	outb(0x20, 0x20);
}

void ISRCODE isr33(struct interrupt_frame* f) // IRQ1
{
	asm volatile("nop");
	outb(0x20, 0x20);
}

void ISRCODE isr34(struct interrupt_frame* f) // IRQ2
{
	asm volatile("nop");
	outb(0x20, 0x20);
}

void ISRCODE isr35(struct interrupt_frame* f) // IRQ3
{
	asm volatile("nop");
	outb(0x20, 0x20);
}

void ISRCODE isr36(struct interrupt_frame* f) // IRQ4
{
	asm volatile("nop");
	outb(0x20, 0x20);
}

void ISRCODE isr37(struct interrupt_frame* f) // IRQ5
{
	asm volatile("nop");
	outb(0x20, 0x20);
}

void ISRCODE isr38(struct interrupt_frame* f) // IRQ6
{
	asm volatile("nop");
	outb(0x20, 0x20);
}

void ISRCODE isr39(struct interrupt_frame* f) // IRQ7
{
	asm volatile("nop");
	outb(0x20, 0x20);
}

void ISRCODE isr40(struct interrupt_frame* f) // IRQ8
{
	asm volatile("nop");
	outb(0x20, 0x20);
	outb(0xA0, 0x20);
}

void ISRCODE isr41(struct interrupt_frame* f) // IRQ9
{
	asm volatile("nop");
	outb(0x20, 0x20);
	outb(0xA0, 0x20);
}

void ISRCODE isr42(struct interrupt_frame* f) // IRQ10
{
	asm volatile("nop");
	outb(0x20, 0x20);
	outb(0xA0, 0x20);
}

void ISRCODE isr43(struct interrupt_frame* f) // IRQ11
{
	asm volatile("nop");
	outb(0x20, 0x20);
	outb(0xA0, 0x20);
}

void ISRCODE isr44(struct interrupt_frame* f) // IRQ12
{
	asm volatile("nop");
	outb(0x20, 0x20);
	outb(0xA0, 0x20);
}

void ISRCODE isr45(struct interrupt_frame* f) // IRQ13
{
	asm volatile("nop");
	outb(0x20, 0x20);
	outb(0xA0, 0x20);
}

void ISRCODE isr46(struct interrupt_frame* f) // IRQ14
{
	asm volatile("nop");
	outb(0x20, 0x20);
	outb(0xA0, 0x20);
}

void ISRCODE isr47(struct interrupt_frame* f) // IRQ15
{
	asm volatile("nop");
	outb(0x20, 0x20);
	outb(0xA0, 0x20);
}
