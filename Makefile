all: init.bin init.elf

CC ?= gcc

CFLAGS := -Wno-builtin-declaration-mismatch -fverbose-asm -masm=intel \
	  -g -fno-stack-protector -fno-stack-check -DIN_KERNEL -O0

OBJs := init.o startup.o mem.o interrupt.o irq.o timer.o conio.o stack_dummy.o \
	paging.o vga_fonts.o vga_cursor.o vga.o vga_modes.o vga_tools.o \
	8042.o floppy.o cmos.o dma.o kcmd.o tree_bst.o kmalloc.o dynlist.o \
	usleep.o

%.o: %.S
	@echo $@
	@as --32 $^ -o $@

interrupt.o: interrupt.c
	@echo $@
	@$(CC) -Wa,-adhln=${^:.c=.s} $(CFLAGS) -mno-red-zone -c -O0 -m32 -fno-pie -mno-80387 $^ -o $@

%.o: %.c
	@echo $@
	@$(CC) -Wa,-adhln=${^:.c=.s} $(CFLAGS) -c -O0 -m32 -fno-pie $^ -o $@

init.bin: $(OBJs)
	@echo $@
	@ld -m elf_i386 -T init.ld $^ -o $@ --oformat=binary

init.elf: $(OBJs)
	@echo $@
	@ld -m elf_i386 -T init.ld $^ -o $@

test: init.elf
	./start.sh

debug: init.elf
	./debug.sh

clean:
	rm -f $(OBJs) *.s init.elf init.bin
