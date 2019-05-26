#include <stdint.h>
#include <malloc.h>
#include "../kmalloc.h"

heap_tree_node heap_tree_root;

heap_tree_node *heap_root = &heap_tree_root;

void print_nodes(heap_tree_node *n)
{
	printf("BST Node at: %08X\n", (uint32_t) n);
	printf("Key = %u\n",  n->node.key);
	printf("a_ptr = %08X\n", (uint32_t) n->a_header.ptr);
	printf("a_ptr_end = %08X\n", (uint32_t) n->a_header.ptr_end);
	printf("f_ptr = %08X\n", (uint32_t) n->f_header.ptr);
	printf("f_ptr_end = %08X\n", (uint32_t) n->f_header.ptr_end);
	dynlist *d = (dynlist *)n->node.data;
	printf("Dynlist (%08X): \n", d);
	while(d) {
		if (d->e) {
			printf(" e = %08X\n", d->e);
			ptr_header *p = ((ptr_header *)d->e)->ptr;
			ptr_header *p_end = ((ptr_header *)d->e)->ptr_end;
			uint32_t size = (uint32_t) p_end - (uint32_t) p;
			printf("  free space: %08X\n", (uint32_t)((ptr_header *)d->e)->ptr);
			printf("  size: %u\n", size);
		}
		d = d->next;
	}
	printf("\n----------\n");
	
	printf("\n--- left ---\n");
	bst_node *left = n->node.left;
	if (left) print_nodes((heap_tree_node *)left);

	printf("\n--- right ---\n");
	bst_node *right = n->node.right;
	if (right) print_nodes((heap_tree_node *)right);

	printf("\n--- up ---\n");
}


void dump_tree(void)
{
	print_nodes(heap_root);


}

int main(void)
{
	// get 1 MB of memory to play
	void *p = malloc(1048576);

	prepare_bst_node(heap_root, 0, (uint32_t)p, 1048576);

	dump_tree();

	void *ptr = kmalloc(131072);
	dump_tree();
	void *ptr2 = kmalloc(65536);
	dump_tree();
	void *ptr3 = kmalloc(32768);
	dump_tree();
	void *ptr4 = kmalloc(32768);
	dump_tree();
	
	kfree(ptr4);
	dump_tree();
	kfree(ptr);
	dump_tree();
	kfree(ptr3);
	dump_tree();


	void *ptr5 = kmalloc(16384);
	dump_tree();

//	void *ptr6 = kmalloc(524288);
	
//	dump_tree();

	kfree(ptr5);
	kfree(ptr2);
	dump_tree(); 

	free(p);

	return 0;
}
