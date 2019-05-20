// #define KERNEL_FUNC __attribute__((__section__(".kernel")))
#include <stdint.h>
void startup(uint32_t magic, uint32_t addr);

#include "printf.h"
#include "mem.h"
#include "interrupt.h"
#include "timer.h"
#include "multiboot.h"

void clrscr_pre(void);
void print_pre(char *dest, char *src);

const char *init_kernel_msg = "Initializing Kernel...";

void dummy2(void) {
	printf("hallo %s", "blabla");
}

const char *ticksymb = "-/|\\";

char buffer[255];

void startup(uint32_t magic, uint32_t addr)
{
	multiboot_info_t *mbi;
	mbi = (multiboot_info_t *)addr;

	clrscr();

	printf(init_kernel_msg);

	if (magic == 0x2BADB002)
		printf("Multiboot magic ok\n");

	mem_init();
	printf("IRQs 0-15 remapped to ISRs 0x20-0x2F\n");
	init_interrupts();
	printf("PIT 8254 initialized to generate IRQ0 with 1 kHz\n");
	init_timer();

	if (mbi->flags & 2)
	{

		printf("mem_lower = %dKB, mem_upper = %dKB\n", mbi->mem_lower,\
			 mbi->mem_upper);
	}

	if (CHECK_FLAG (mbi->flags, 6))
         {
           multiboot_memory_map_t *mmap;
		printf("mmap_addr = 0x%x, length = 0x%x\n",
			mbi->mmap_addr, mbi->mmap_length);

             for (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
                (uint32_t) mmap < mbi->mmap_addr + mbi->mmap_length;
                mmap = (multiboot_memory_map_t *) ((uint32_t) mmap
                                         + mmap->size + sizeof (mmap->size)))
            	{
			printf(" size = 0x%x, base = 0x%x, len = 0x%x, type = 0%u\n",
				mmap->size, mmap->addr0 & 0xFFFFFFFF,
				mmap->len0 & 0xFFFFFFFF, mmap->type);
		}
         }


	asm volatile ("sti");
	while(1) {
		*(char *)(VIDEO + 158) = ticksymb[(syscounter >> 6) % 4];
		*(char *)(VIDEO + 159) = 0x07;
		asm volatile("nop");
	}
}

void clrscr_pre(void)
{
	unsigned short *scrmem = (unsigned short *)VIDEO;	

	for (unsigned long i = 0; i < 80*25; i++) {
		*scrmem = 0x0720;
		scrmem++;
	}
}

void print_pre(char *dest, char *src)
{
	while (*src) {
		*dest = *src;
		dest += 2;
		src++;
	}	
}

