#include "mem.h"

#pragma(__align(32))
struct GDT gdt[12];
struct GDT idt[32];

struct __attribute__((packed)) GDTDESC {
	uint16_t limit;
	uint32_t base;
} gdtdesc;

void rmemset(void *dest, uint8_t val, uint32_t size)
{
	uint8_t *d = (uint8_t *)dest;
	for (uint32_t i = 0; i < size; i++, d++)
	{
		*d = val;
	}
}

void dummy(void) {
	asm volatile("nop");
}

#ifdef IN_KERNEL
void mem_init(void)
{
	rmemset(gdt, 0, sizeof(gdt));
	//			base        G     AVL   DPL
	//			    limit       D     P     TYPE
	// kernel code
	init_descriptor(&gdt[1], 0, 0xFFFFF, 1, 1, 0, 1, 0, EXE_SEG | EXE_READABLE);
	// kernel data
	init_descriptor(&gdt[2], 0, 0xFFFFF, 1, 1, 0, 1, 0, DATA_SEG | DATA_WRITEABLE);
	// global data
	init_descriptor(&gdt[3], 0, 0xFFFFF, 1, 1, 0, 1, 0, DATA_SEG | DATA_WRITEABLE);

	gdtdesc.limit = 4*sizeof(struct GDT) - 1;
	gdtdesc.base = (uint32_t) &gdt[0];

	asm volatile(
		"lgdt [eax]"::"a"(&gdtdesc)
	);
	dummy();
	asm volatile(
		"mov ax, 0x10\n"
		"mov ds, ax\n"
		"mov es, ax\n"
		"mov fs, ax\n"
		"mov ss, ax\n"
		"mov ax, 0x18\n"
		"mov gs, ax\n":::"memory", "%eax"
	);
}
#endif

void init_descriptor(struct GDT *desc, uint32_t base,
	uint32_t limit, uint8_t granularity, uint8_t datasize,
	uint8_t available, uint8_t present, uint8_t dpl, uint8_t type)
{
	desc->base_lo = (base & 0xFFFF);
	desc->base_mid = ((base >> 16) & 0xFF);
	desc->base_hi = ((base >> 24) & 0xFF);

	desc->limit_lo = (limit & 0xFFFF);
	desc->limit_hi = ((limit >> 16) & 0xFF);

	desc->granularity = granularity;
	desc->datasize = datasize;
	desc->available = available;
	desc->present = present;
	desc->dpl = dpl;
	desc->type = type;

	desc->reserved = 0;
}
