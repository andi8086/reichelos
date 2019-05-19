all: init.bin init.elf

CFLAGS := -Wno-builtin-declaration-mismatch -fverbose-asm -masm=intel

OBJs := init.o startup.o mem.o interrupt.o irq.o timer.o printf.o stack_dummy.o

%.o: %.S
	as --32 $^ -o $@

interrupt.o: interrupt.c
	gcc -Wa,-adhln=${^:.c=.s} $(CFLAGS) -mno-red-zone -c -O0 -m32 -fno-pie -mno-80387 $^ -o $@

%.o: %.c
	gcc -Wa,-adhln=${^:.c=.s} $(CFLAGS) -c -O0 -m32 -fno-pie $^ -o $@

init.bin: $(OBJs)
	ld -m elf_i386 -T init.ld $^ -o $@ --oformat=binary
	
init.elf: $(OBJs)
	ld -m elf_i386 -T init.ld $^ -o $@

test: init.elf
	./start.sh

debug: init.elf
	./debug.sh

clean:
	rm -f $(OBJs) init.elf init.bin
