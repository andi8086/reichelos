#ifndef __MEM_H__
#define __MEM_H__

#include <stdint.h>

#define DESCTYPE_TSS_286_AVAIL	1
#define DESCTYPE_LDT		2
#define DESCTYPE_TSS_286_BUSY	3
#define DESCTYPE_CALL_GATE	4
#define DESCTYPE_TASK_GATE	5
#define DESCTYPE_INT_GATE_286	6
#define DESCTYPE_TRAP_GATE_286  7
#define DESCTYPE_TSS_386_AVAIL	9
#define DESCTYPE_TSS_386_BUSY	11
#define DESCTYPE_CALL_GATE_386	12
#define DESCTYPE_INT_GATE_386	14
#define DESCTYPE_TRAP_GATE_386	15

#define EXE_SEG			24
#define EXE_CONFORMING		4
#define EXE_READABLE		2
#define EXE_ACCESSED		1

#define DATA_SEG		16
#define DATA_EXPANDDOWN		4
#define DATA_WRITEABLE		2
#define DATA_ACCESSED		1

struct __attribute__((packed)) GDT {
	uint16_t limit_lo;
	uint16_t base_lo;
	uint8_t  base_mid;
	unsigned type: 5;
	unsigned dpl: 2;
	unsigned present: 1;
	unsigned limit_hi: 4;
	unsigned available: 1;
	unsigned reserved: 1;
	unsigned datasize: 1;
	unsigned granularity: 1;
	uint8_t  base_hi;
};

void mem_init(void);
void rmemset(void *dest, uint8_t val, uint32_t size);
void init_descriptor(struct GDT *desc, uint32_t base, uint32_t limit, uint8_t granularity, uint8_t datasize, uint8_t available, uint8_t present, uint8_t dpl, uint8_t type);


#endif // __MEM_H__
