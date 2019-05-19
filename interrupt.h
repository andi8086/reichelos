#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <stdint.h>

struct interrupt_frame;

struct __attribute__((packed)) IDT {
	uint16_t offset_lo;
	uint16_t selector;
	uint8_t ist;
	unsigned type: 5;
	unsigned dpl: 2;
	unsigned present: 1;
	uint16_t offset_mid;
};

void init_interrupts(void);
void init_interrupt(struct IDT *desc, uint16_t selector, uint32_t offset,
		    uint8_t type, uint8_t dpl);

#define ISRCODE __attribute__((interrupt,\
			target("no-sse","no-mmx","no-fancy-math-387")))

void ISRCODE isr0(struct interrupt_frame* f); // divide-by-zero
void ISRCODE isr1(struct interrupt_frame* f); // debug
void ISRCODE isr2(struct interrupt_frame* f); // NMI
void ISRCODE isr3(struct interrupt_frame* f); // breakpoint
void ISRCODE isr4(struct interrupt_frame* f); // overflow
void ISRCODE isr5(struct interrupt_frame* f); // bound range exceed
void ISRCODE isr6(struct interrupt_frame* f); // invalid opcode
void ISRCODE isr7(struct interrupt_frame* f); // device not available
void ISRCODE isr8(struct interrupt_frame* f, uint32_t err); // double fault
void ISRCODE isr9(struct interrupt_frame* f); // coprocessor seg overrun
void ISRCODE isr10(struct interrupt_frame* f, uint32_t err); // invalid tss
void ISRCODE isr11(struct interrupt_frame* f, uint32_t err); // seg not present
void ISRCODE isr12(struct interrupt_frame* f, uint32_t err); // stack-seg fault
void ISRCODE isr13(struct interrupt_frame* f, uint32_t err); // gp
void ISRCODE isr14(struct interrupt_frame* f, uint32_t err); // page fault
void ISRCODE isr15(struct interrupt_frame* f); // reserved
void ISRCODE isr16(struct interrupt_frame* f); // x87 fpu exception 
void ISRCODE isr17(struct interrupt_frame* f, uint32_t err); // alignment check
void ISRCODE isr18(struct interrupt_frame* f); // machine check
void ISRCODE isr19(struct interrupt_frame* f); // SIMD FPU exception
void ISRCODE isr20(struct interrupt_frame* f); // virtualiation exception
void ISRCODE isr21(struct interrupt_frame* f); // reserved
void ISRCODE isr22(struct interrupt_frame* f); // reserved
void ISRCODE isr23(struct interrupt_frame* f); // reserved
void ISRCODE isr24(struct interrupt_frame* f); // reserved
void ISRCODE isr25(struct interrupt_frame* f); // reserved
void ISRCODE isr26(struct interrupt_frame* f); // reserved
void ISRCODE isr27(struct interrupt_frame* f); // reserved
void ISRCODE isr28(struct interrupt_frame* f); // reserved
void ISRCODE isr29(struct interrupt_frame* f); // reserved
void ISRCODE isr30(struct interrupt_frame* f, uint32_t err); // security exception
void ISRCODE isr31(struct interrupt_frame* f); // reserved

void ISRCODE isr32(struct interrupt_frame* f); // IRQ0
void ISRCODE isr33(struct interrupt_frame* f); // IRQ1
void ISRCODE isr34(struct interrupt_frame* f); // IRQ2
void ISRCODE isr35(struct interrupt_frame* f); // IRQ3
void ISRCODE isr36(struct interrupt_frame* f); // IRQ4
void ISRCODE isr37(struct interrupt_frame* f); // IRQ5
void ISRCODE isr38(struct interrupt_frame* f); // IRQ6
void ISRCODE isr39(struct interrupt_frame* f); // IRQ7
void ISRCODE isr40(struct interrupt_frame* f); // IRQ8
void ISRCODE isr41(struct interrupt_frame* f); // IRQ9
void ISRCODE isr42(struct interrupt_frame* f); // IRQ10
void ISRCODE isr43(struct interrupt_frame* f); // IRQ11
void ISRCODE isr44(struct interrupt_frame* f); // IRQ12
void ISRCODE isr45(struct interrupt_frame* f); // IRQ13
void ISRCODE isr46(struct interrupt_frame* f); // IRQ14
void ISRCODE isr47(struct interrupt_frame* f); // IRQ15


#endif // __INTERRUPT_H__
