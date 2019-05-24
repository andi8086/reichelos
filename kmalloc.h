#ifndef __MALLOC_H__
#define __MALOC_H__

typedef struct __attribute__((packed)) {
    void *ptr;      // beginning of the memory chunk
    void *ptr_end;  // end of the memory chunk
} ptr_header;

void heap_init(void);
void *kmalloc(uint32_t size);
void kfree(void *p);

#endif
