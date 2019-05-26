#ifndef __TREE_BST_H__
#define __TREE_BST_H__

#include <stdint.h>

// A binary search tree uses a criteria to order its elements
// In case of the memory allocator, this is the free block size

typedef struct __attribute__((packed)) _bst_node {
    struct _bst_node *parent;
    struct _bst_node *left;
    struct _bst_node *right;
    uint32_t key;
    void* data;
} bst_node;

bst_node *bst_search_smallest_ge(bst_node *node, uint32_t key);
bst_node **bst_insert(bst_node **root, uint32_t key);
bst_node **bst_find_min(bst_node **node);
void bst_replace_node_in_parent(bst_node **node, bst_node *new_node);
void bst_delete_node(bst_node **node, uint32_t key);

#endif // __TREE_BST_H__
