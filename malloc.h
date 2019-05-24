#ifndef __MALLOC_H__
#define __MALOC_H__

typedef struct __attribute__((packed)) {
    void *ptr;      // beginning of the memory chunk
    void *ptr_end;  // end of the memory chunk
    void *free_prev;// next free block with lower address
    void *free_next;// next free block with higher address
} ptr_header;

void heap_init(void);

#endif
