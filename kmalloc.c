#include "mem.h"
#include "kmalloc.h"
#include "kernel.h"
#include "kprintf.h"

#ifdef DEBUG
#include <stdio.h>
#endif

// the root node of the heap manager binary search tree
#ifdef IN_KERNEL
heap_tree_node heap_tree_root;

heap_tree_node *heap_root = &heap_tree_root;
#endif

void prepare_bst_node(heap_tree_node *htn, uint32_t a_size,
		     uint32_t f_start, uint32_t f_size)
{
	#ifdef DEBUG
	printf("prepare_bst_node: new node at %08X\n", htn);
	#endif

	htn->pointerlist.next = 0;
	htn->pointerlist.prev = 0;
	htn->pointerlist.e = &htn->f_header;

	htn->f_header.ptr = (void *)f_start;
	htn->f_header.ptr_end = (void *)(f_start + f_size);

	if (a_size == 0) a_size += sizeof(heap_tree_node);
	htn->a_header.ptr = (void *)((uint32_t)htn + sizeof(heap_tree_node));
	htn->a_header.ptr_end = (void *)((uint32_t)htn + a_size);

	htn->node.key = f_size;
	htn->node.parent = 0;
	htn->node.right = 0;
	htn->node.left = 0;
	htn->node.data = (void *)&htn->pointerlist;
}

void heap_init(void)
{
	prepare_bst_node(heap_root, 0, 0x150000, 1048576);
	KPRINTF("Memory manager initialized.\n");
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
		
		#ifdef DEBUG
		printf("   dynlist_add: newnode->data->e->f_header.ptr = %08X\n",
			((ptr_header *)((dynlist *)(*f_node)->data)->e)->ptr);
		printf("   dynlist_add: newnode->data->e->f_header.ptr_end = %08X\n",
			((ptr_header *)((dynlist *)(*f_node)->data)->e)->ptr_end);
		#endif


	} else {
		// there is already a node with the same key, so add the
		// ptr_header to its data list
		dynlist_prepend((dynlist **)&(*f_node)->data, newnode->data);

		#ifdef DEBUG
		printf("   dynlist_prepend: newnode->data->e->f_header.ptr = %08X\n",
			((ptr_header *)((dynlist *)(*f_node)->data)->e)->ptr);
		printf("   dynlist_prepend: newnode->data->e->f_header.ptr_end = %08X\n",
			((ptr_header *)((dynlist *)(*f_node)->data)->e)->ptr_end);
		#endif

		// erase the BST field, since we are not the master BST node
		//rmemset(newnode, 0, sizeof(bst_node));
	}
	return f_node;
}

void kfree(void *ptr)
{
	#ifdef DEBUG
	printf(" ********** kfree **********\n");
	#endif
	heap_tree_node *htn = (heap_tree_node *)((uint32_t )ptr -
			       sizeof(heap_tree_node));

	bst_node *node = &(htn->node);

	#ifdef DEBUG
	printf("kfree: %d, node = %08X\n", __LINE__, node);
	#endif	
	uint32_t a_ptr = (uint32_t)htn->a_header.ptr;
	uint32_t a_ptr_end = (uint32_t)htn->a_header.ptr_end;
	uint32_t f_ptr = (uint32_t)htn->f_header.ptr;
	uint32_t f_ptr_end = (uint32_t)htn->f_header.ptr_end;

	dynlist *ptrlist = (dynlist *)node->data;
	#ifdef DEBUG
	printf("kfree: dynlist = %08X\n", ptrlist);
	#endif
	if (!ptrlist) {
		KPRINTF("kfree: error, dynlist is NULL\n");
		return;
	}
	#ifdef DEBUG
	uint32_t of_ptr = (uint32_t)((ptr_header *)ptrlist->e)->ptr;
	uint32_t of_ptr_end = (uint32_t)((ptr_header *)ptrlist->e)->ptr_end;
	printf("kfree: f_ptr = %08X\n", of_ptr);
	uint32_t osize =  of_ptr_end - of_ptr;
	printf("kfree: f_ptr_end = +%u\n", osize);
	#endif

	heap_tree_node *new_htn = (heap_tree_node *)ptr;
	bst_node *new_node = &new_htn->node;
		
	// set f_header to a_header and set a_header to 0
	new_htn->f_header.ptr = 
		(void *)((uint32_t) ptr + sizeof(heap_tree_node));
	new_htn->f_header.ptr_end = htn->a_header.ptr_end;
	new_htn->a_header.ptr = 0;
	new_htn->a_header.ptr_end = 0;
	new_node->key = a_ptr_end - a_ptr - sizeof(heap_tree_node);

	#ifdef DEBUG
	printf("kfree: new_htn->pointerlist = %08X\n", &new_htn->pointerlist);
	#endif
	new_htn->pointerlist.e = &new_htn->f_header;
	new_htn->pointerlist.next = 0;
	new_htn->pointerlist.prev = 0;
	new_node->data = (void *)&new_htn->pointerlist;
	heap_add_chunk(new_node->key, new_node);

	return;
}


void *kmalloc(uint32_t size)
{
	#ifdef DEBUG
	printf(" ********** kmalloc **********\n");
	#endif
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

	// check if the BST node has a valid dynamic pointer list

	dynlist *old_ptr_list = (dynlist *)old_node->data;
	if (!old_ptr_list) {
		KPRINTF("heap corruption! Zombie BST node\n");
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

	#ifdef DEBUG
	printf("kmalloc: prepare_bst_node(a_ptr = %08X, a_size = %08X, f_ptr = %08X, f_size = %08X)\n",
		ptr_start, alloc_size, ptr_start+alloc_size, f_size);
	#endif
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
	// pointer, (BUGFIX: use ptr_start instead of old_node,
	// otherwise, we modify f_header belonging to another chunk) 
	//ptr_header *p = &((heap_tree_node *)(uint32_t)ptr_start - sizeof(heap_tree_node))->f_header;
	//p->ptr_end = p->ptr;
	
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
			bst_delete_node((bst_node **)&heap_root, old_node->key);
			old_node->key = 0;
		}
	}
	// the pointer to be returned must point to the beginning of the
	// allocated chunk

	void *ret_ptr = ((heap_tree_node *)ptr_start)->a_header.ptr;
	return ret_ptr;
}
