#include "tree_bst.h"
#include "kmalloc.h"
#include "kernel.h"
#include "dynlist.h"
#include "conio.h"

typedef struct __attribute__((packed)) {
	bst_node node;
	dynlist pointerlist;
	ptr_header f_header;
	ptr_header a_header;
} heap_tree_node;

// the root node of the heap manager binary search tree
heap_tree_node heap_tree_root;

heap_tree_node *heap_root = &heap_tree_root;


void prepare_bst_node(heap_tree_node *htn, uint32_t a_size,
		     uint32_t f_start, uint32_t f_size)
{
	htn->pointerlist.next = 0;
	htn->pointerlist.prev = 0;
	htn->pointerlist.e = &htn->f_header;

	htn->f_header.ptr = (void *)f_start;
	htn->f_header.ptr_end = (void *)(f_start + f_size);

	htn->a_header.ptr = (void *)((uint32_t)htn + sizeof(heap_tree_node));
	htn->a_header.ptr_end = (void *)((uint32_t) htn + a_size);

	htn->node.key = f_size;
	htn->node.parent = 0;
	htn->node.right = 0;
	htn->node.left = 0;
	htn->node.data = (void *)&htn->pointerlist;
}

void heap_init(void)
{
	prepare_bst_node(heap_root, 0, 0x150000, 1048576);
	printf("Memory manager initialized.\n");
}

void kmemcpy(uint8_t *dest, uint8_t *src, uint32_t count)
{
	for (uint32_t i = 0; i < count; i++) {
		*dest = *src;
		dest++; src++;
	}
}

void kfree(void *ptr)
{
	heap_tree_node *htn = (heap_tree_node *)((uint32_t )ptr -
			       sizeof(heap_tree_node));

	printf("kfree: htn = %x\n", htn);

	bst_node *node = &(htn->node);

	printf("kfree: mem @ %x\n", htn->a_header.ptr);
	printf("kfree: size: %u\n", (uint32_t)htn->a_header.ptr_end -
				    (uint32_t)htn->a_header.ptr);

	// create a new bst node


}


void *kmalloc(uint32_t size)
{
	// find closest match in bst
	uint32_t alloc_size = size + sizeof(heap_tree_node);
	bst_node *node = bst_search_smallest_ge(&heap_root->node, alloc_size);

	if (!node) {
		return 0;
	}
	uint32_t key = node->key;
	printf("closest BST tree node at %x for %u byte chunk\n",
	       (uint32_t) node, key);

	// we have a node, with memory starting at *(node->data[]->e->ptr)
	dynlist *ptr_list = (dynlist *)node->data;
	if (!ptr_list) {
		printf("heap corruption! Zombie BST node\n");
		return 0;
	}
	ptr_header *chunk_header = (ptr_header *)ptr_list->e;
	uint32_t ptr_start = (uint32_t)chunk_header->ptr;
	uint32_t ptr_end = (uint32_t)chunk_header->ptr_end;

	uint32_t f_size = key - alloc_size;

	prepare_bst_node((heap_tree_node *)ptr_start, alloc_size,
			 ptr_start + alloc_size, f_size);

	if (f_size == 0) goto malloc_finish;

	// the new node has already been prepared
	bst_node *newnode = &((heap_tree_node *)ptr_start)->node;

	bst_node *f_node = bst_insert(&heap_root->node, f_size, newnode);

	//f_node->key = f_size;

	// f_node is either new or an already existing BST node
	printf("New BST node at %x\n", f_node);

	if (!f_node->data) {
		printf("Error creating new f_node->data\n");
	}
malloc_finish:
	// delete the pointer header from the dynlist
	dynlist_del((dynlist **)&(node->data));
	if (!node->data) {
		// this was the last pointer header in the dynlist, remove the BST node
		if (node == &heap_root->node) {
			kmemcpy((uint8_t *)&heap_root->node,
				(uint8_t *)f_node, sizeof(bst_node));
		} else {
			bst_delete_node(&heap_root->node, key);
		}
	}

	void *ret_ptr = ((heap_tree_node *)ptr_start)->a_header.ptr;

	printf("a_header.ptr = %x\n", ret_ptr);
	return ret_ptr;
}
