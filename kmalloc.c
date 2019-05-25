#include "tree_bst.h"
#include "mem.h"
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
		
bst_node **heap_add_chunk(uint32_t f_size, bst_node *newnode)
{
	// just tell that we want to insert a new node with key = f_size
	bst_node **f_node = bst_insert((bst_node **)&heap_root, f_size);

	if (!*f_node) {
		// there is no node, but the pointer points to the place in the
		// tree where it must be added
		*f_node = newnode;
		printf("New BST node at %x\n", *f_node);
	} else {
		// there is already a node with the same key, so add the
		// ptr_header to its data list
		dynlist_prepend((dynlist **)&(*f_node)->data, newnode->data);
		// erase the BST field, since we are not the master BST node
		rmemset(newnode, 0, sizeof(bst_node));
	}
	return f_node;
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

	// create a new bst node describing the freed memory
	// by overwriting the existing bst node describing the allocated
	// chunk, deleting it from the tree and re-adding it to keep the
	// structure
	
	uint32_t a_ptr = (uint32_t)htn->a_header.ptr;
	uint32_t a_ptr_end = (uint32_t)htn->a_header.ptr_end;
	uint32_t f_ptr = (uint32_t)htn->f_header.ptr;
	uint32_t f_ptr_end = (uint32_t)htn->f_header.ptr_end;

	dynlist **ptrlist = (dynlist **)&node->data;
	if (!*ptrlist) {
		printf("kfree: error, dynlist is NULL\n");
		return;
	}
	// check if this is a master BST node, linking to other nodes with the
	// same key

	if (!node->key) goto kfree_no_master_BST;
	
	// this is a master BST, look if there are more elements in the dynlist
	// than 1
	if (!(*ptrlist)->next) goto kfree_no_linked_ptrs;

	// we must swap with a slave chunk:
	// 	1. delete the node from the BST
	//	2. remove the prev pointer of the next element in the dynlist
	//	3. recreate the BST node with the memory around the next element
	//	4. add the BST
	//	5. -> we are free to use this memory chunk :D

	bst_delete_node(node, node->key);
	(*ptrlist)->next->prev = 0;
	
	bst_node *next_node =
		(bst_node *)((uint32_t)(*ptrlist)->next - sizeof(bst_node));
	next_node->data = (*ptrlist)->next;
	next_node->key = node->key;	
	(*ptrlist)->next = 0;

	// Delete the node from the BST
	bst_delete_node(node, node->key);
	
	// add the adjusted one
	heap_add_chunk(node->key, next_node);
	
	goto kfree_finish;

kfree_no_linked_ptrs:
	// Delete the node from the BST
	bst_delete_node(node, node->key);

	goto kfree_finish;

kfree_no_master_BST:

	// remove from the linked list
	if (!(*ptrlist)->next) goto kfree_no_linked_ptrs;

	(*ptrlist)->next->prev = (*ptrlist)->prev;

	if ((*ptrlist)->prev) {
		(*ptrlist)->prev->next = (*ptrlist)->next;
	}

kfree_finish:
	// initialize a new bst node in this memory chunk to be a free chunk
	htn->f_header.ptr = (void *)a_ptr;
	htn->a_header.ptr = 0;
	htn->a_header.ptr_end = 0;
	htn->node.key = f_ptr_end - a_ptr;
	
	heap_add_chunk(htn->node.key, &htn->node);	
}

void *kmalloc(uint32_t size)
{
	// find closest match in bst
	uint32_t alloc_size = size + sizeof(heap_tree_node);

	// search the BST to find a node that desribes a chunk with
	// key >= alloc_size

	bst_node *old_node =
		bst_search_smallest_ge(&heap_root->node, alloc_size);

	if (!old_node) {
		// we could not find any, no memory available
		return 0;
	}

	printf("closest BST tree node at %x for %u byte chunk\n",
	       (uint32_t) old_node, old_node->key);

	// check if the BST node has a valid dynamic pointer list

	dynlist *old_ptr_list = (dynlist *)old_node->data;
	if (!old_ptr_list) {
		printf("heap corruption! Zombie BST node\n");
		return 0;
	}

	// The BST node may have several pointer headers,
	// describing a memory chunk with the same size.
	// Take the first one. The element is always an f_header.

	ptr_header *chunk_header = (ptr_header *)old_ptr_list->e;

	// extract the f_header pointers

	uint32_t ptr_start = (uint32_t)chunk_header->ptr;
	uint32_t ptr_end = (uint32_t)chunk_header->ptr_end;

	// calculate the free size of the remaining space after
	// subtracting the space for the header information

	uint32_t f_size = old_node->key - alloc_size;

	// prepare a new BST node describing the remaining free memory
	// together with the allocated part

	prepare_bst_node((heap_tree_node *)ptr_start, alloc_size,
			 ptr_start + alloc_size, f_size);

	// if the remaining free part is 0, we don't insert it into the BST
	// and continue with the finishing part

	if (f_size == 0) goto malloc_finish;

	// the new node has already been prepared, prepare adding it to the BST
	// the whole memory of the new node is always inside a free region
	bst_node *newnode = &((heap_tree_node *)ptr_start)->node;


	bst_node **f_node = heap_add_chunk(f_size, newnode);




malloc_finish:
	// delete the pointer header from the dynlist
	dynlist_del((dynlist **)&(old_node->data));
	// furthermore, we must set the f_header end pointer to the start
	// pointer
	ptr_header *p = ((ptr_header *)((dynlist *)old_node->data)->e);
	p->ptr_end = p->ptr;
	
	if (!old_node->data) {
		// this was the last pointer header in the dynlist, remove the
		// BST node
		if (old_node == &heap_root->node) {
			// we replace the data of the root node with the data
			// of the new node, hence deleting the root node by
			// overwriting
			kmemcpy((uint8_t *)old_node, (uint8_t *)*f_node,
				sizeof(bst_node));
		} else {
			// we delete a node in the BST
			bst_delete_node(&heap_root->node, old_node->key);
			old_node->key = 0;
		}
	}
	// the pointer to be returned must point to the beginning of the
	// allocated chunk
	void *ret_ptr = ((heap_tree_node *)ptr_start)->a_header.ptr;

	printf("a_header.ptr = %x\n", ret_ptr);
	return ret_ptr;
}
