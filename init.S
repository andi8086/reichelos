.intel_syntax noprefix

.equ MULTIBOOT_HEADER, 0x1badb002

.equ STACK_SIZE, 0x1000

.text

/************************** START OF PROGRAM *********************
 * If the program is to be invoked, i.e. from MS-DOS, it can be done by
 * loading the image to 0x1000 and just jumping to the beginning.
 * This is 16-bit code and begins initialization with _init16.
 *
 * However, if the image is loaded by a bootloader that supports the
 * multiboot header, then the multiboot_entry is used, where the CPU
 * already is in protected mode with A20 enabled.
 */

	.extern startup;

.code16
start:
_start:
	jmp _init16
	.align 4

.code32
multiboot_header:
	.long MULTIBOOT_HEADER
	.long 0x10000		/* use loader information in header */
	/* checksum */
	.long - (MULTIBOOT_HEADER + 0x10000)
	.long multiboot_header  /* loader information */
	.long _start
	.long _LOAD_END_
	.long _end
	.long multiboot_entry
header_end:

multiboot_entry:
//	lgdt [GDT_DESC]

//	jmp 0x08:rock_bottom
//rock_bottom:
//	mov ax, 0x10
//	mov ds, ax
//	mov es, ax
//	mov ss, ax
//	mov fs, ax
//	mov gs, ax

	mov esp, offset stack + STACK_SIZE - 1
	
//	mov byte ptr es:[0xB8000],'A'	
//	mov byte ptr es:[0xB8001], 0x27

	push ebx
	push eax
	call startup
	add esp, 8

	mov byte ptr es:[0xB8004], 'R'
	mov byte ptr es:[0xB8005], 0x27
1:
	hlt
	jmp 1b

.code16

_init16:
1:
	jmp 1b

.code32

.align 32
GDT_DESC:.hword INIT_GDT_END - INIT_GDT - 1
	.long INIT_GDT

.align 32
INIT_GDT:
	.long 0
	.long 0
IGCSFLAT: .hword 0xffff
	.hword 0
	.byte 0
	.byte 0x9A
	.byte 0xCF
	.byte 0

IGDSFLAT: .hword 0xffff
	.hword 0
	.byte 0
	.byte 0x92
	.byte 0xCF
	.byte 0
INIT_GDT_END:

	.comm stack, STACK_SIZE
