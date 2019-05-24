#include "tree_bst.h" 
#include "malloc.h"
#include "kernel.h"
#include "dynlist.h"
#include "conio.h"

// the root node of the heap manager binary search tree
bst_node heap_tree_root;
ptr_header root_ptr_header;
dynlist pointerlist;

bst_node *heap_root = &heap_tree_root;

void heap_init(void)
{
    pointerlist.next = 0;
    pointerlist.prev = 0;
    pointerlist.e = &root_ptr_header;

    // in the beginning we have one big chunk that is free
    root_ptr_header.ptr = (void *)0x150000;
    root_ptr_header.ptr_end = (void *)0x250000;

    heap_tree_root.key = 1048576;   // 1M free
    heap_tree_root.parent = 0;
    heap_tree_root.right = 0;
    heap_tree_root.left = 0;
    heap_tree_root.data = (void *)&pointerlist;  

    printf("Memory manager initialized.\n");
}

void *malloc(uint32_t size)
{
    // find closest match in bst
    uint32_t alloc_size = size + sizeof(bst_node) + sizeof(dynlist) +
                          sizeof(ptr_header)*2;
    bst_node *node = bst_search_smallest_ge(heap_root, alloc_size);

    if (!node) {
        return 0;
    }
    uint32_t key = node->key;
    printf("closest BST tree node at %x for %u byte chunk\n", (uint32_t) node,               key);

    // we have a node, with memory starting at node->data[]->e->ptr;
    dynlist *ptr_list = (dynlist *)node->data;
    if (!ptr_list) {
        printf("heap corruption! Zombie BST node\n");
        return 0;
    }
    ptr_header *chunk_header = (ptr_header *)ptr_list->e;
    uint32_t ptr_start = (uint32_t) chunk_header->ptr;
    uint32_t ptr_end = (uint32_t) chunk_header->ptr_end;

    // create a new pointer header for the allocated space
    ptr_header *a_header = (ptr_header *) (ptr_start + sizeof(bst_node) +
                                           sizeof(dynlist) + sizeof(ptr_header));

    a_header->ptr_end = (void *)(ptr_start + alloc_size);
    a_header->ptr = (void *)(ptr_start + (alloc_size - size));

    // create a new pointer header for the remainder of the chunk
    ptr_header *f_header = (ptr_header *) (ptr_start + sizeof(bst_node) +
                                           sizeof(dynlist));

    f_header->ptr_end = (void *)(ptr_end);
    f_header->ptr = (void *)(ptr_start + alloc_size + 1);
    uint32_t f_size = key - alloc_size;
    
    if (f_size == 0) goto malloc_finish; 
    // create a new dynlist for the BST node
    // (it's for the kernel, but remains in the allocated user space)
    dynlist *alloclist = (dynlist *) (ptr_start + sizeof(bst_node));
    alloclist->next = 0;
    alloclist->prev = 0;
    alloclist->e = (void *)f_header; 

    // first create a new BST node, which is there anyway
    bst_node *newnode = (bst_node *) ptr_start; 

    bst_node *f_node = bst_insert(heap_root, f_size, newnode);

    f_node->key = f_size;

    // f_node is either new or an already existing BST node
    printf("New BST node at %x\n", f_node);
    dynlist_prepend((dynlist **)&(f_node->data), alloclist);
    if (!f_node->data) {
        printf("Error creating new f_node->data\n");
    }    
malloc_finish: 
    // nothing should go wrong now!
    // delete the pointer header from the dynlist
    dynlist_del((dynlist **)&(node->data)); 
    if (!node->data) {
        // this was the last pointer header in the dynlist, so remove the BST
        // node
        if (node == heap_root) {
            heap_root = f_node;
        } else {
            bst_delete_node(heap_root, key);
        }
    }
    return a_header->ptr; 

}
