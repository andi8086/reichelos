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

#endif // __TREE_BST_H__
