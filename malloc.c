#include "tree_bst.h" 
#include "malloc.h"
#include "kernel.h"
#include "dynlist.h"
#include "conio.h"

// the root node of the heap manager binary search tree
bst_node heap_tree_root;
ptr_header root_ptr_header;
dynlist pointerlist;

void heap_init(void)
{
    pointerlist.next = 0;
    pointerlist.prev = 0;
    pointerlist.e = &root_ptr_header;

    // in the beginning we have one big chunk that is free
    root_ptr_header.ptr = (void *)0x150000;
    root_ptr_header.ptr_end = (void *)0x250000;
    root_ptr_header.free_prev = (ptr_header *)0;
    root_ptr_header.free_next = (ptr_header *)0;

    heap_tree_root.key = 1048576;   // 1M free
    heap_tree_root.parent = 0;
    heap_tree_root.right = 0;
    heap_tree_root.left = 0;
    heap_tree_root.data = (void *)&pointerlist;  

    printf("Memory manager initialized.\n");
}

