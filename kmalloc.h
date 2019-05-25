#ifndef __MALLOC_H__
#define __MALOC_H__

#include <stdint.h>
#include "dynlist.h"
#include "tree_bst.h"

typedef struct __attribute__((packed)) {
    void *ptr;      // beginning of the memory chunk
    void *ptr_end;  // end of the memory chunk
} ptr_header;

typedef struct __attribute__((packed)) {
	bst_node node;
	dynlist pointerlist;
	ptr_header f_header;
	ptr_header a_header;
} heap_tree_node;

extern heap_tree_node heap_tree_root; 
extern heap_tree_node *heap_root;

void heap_init(void);
void prepare_bst_node(heap_tree_node *htn, uint32_t a_size,
		     uint32_t f_start, uint32_t f_size);
void *kmalloc(uint32_t size);
void kfree(void *p);

#endif
