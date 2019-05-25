#include "tree_bst.h"

#ifdef DEBUG
#include <stdio.h>
#endif

// returns the node whose critearis is the smallest one which is greater or
// equal to the requested one
bst_node *bst_search_smallest_ge(bst_node *node, uint32_t key)
{
    bst_node *current = node;
    uint32_t smallest_delta = 0xFFFFFFFF;
    bst_node *best_match = 0;
    while (current) {
        if (key == current->key) {
            // we found one that exactly matches
            return current;
        }
        if (key < current->key) {
            // this one is too big, but remember it if we don't find a better
            // one
            smallest_delta = current->key - key;
            best_match = current;
            // look if there is a smaller one that is greater or equal
            current = current->left;
        } else {
            // this one is too small, so go right
            current = current->right;
        }
    }
    return best_match;
}

// returns the insert location, where the node data has to go.
// Either there exists a node, or not.
bst_node **bst_insert(bst_node **root, uint32_t key)
{
    bst_node **walk = root;
    while (*walk) {
        uint32_t currkey = (*walk)->key;
        if (currkey == key) {
            return walk;
        }
        if (key > currkey) {
            walk = &(*walk)->right;
        } else {
            walk = &(*walk)->left;
        }
    }
    return walk;
}

bst_node *bst_find_min(bst_node *node)
{
    bst_node *current_node = node;
    while (current_node->left) current_node = current_node->left;
    return current_node;
}

void bst_replace_node_in_parent(bst_node *node, bst_node *new_node)
{
    if (node->parent) {
        if (node == node->parent->left) {
            node->parent->left = new_node;
        } else {
            node->parent->right = new_node;
        }
    } else {
	// this is a root node
	
    }
    if (new_node) {
        new_node->parent = node->parent;
    }    
}

void bst_delete_node(bst_node *node, uint32_t key)
{
    if (key < node->key) {
        bst_delete_node(node->left, key);
        return;
    }
    if (key > node->key) {
        bst_delete_node(node->right, key);
        return;
    }
    // delete the key here

    if (node->left && node->right) {
        bst_node *successor = bst_find_min(node->right);
        node->key = successor->key;
        bst_delete_node(successor, successor->key);
    } else if (node->left) {
        bst_replace_node_in_parent(node, node->left); 
    } else if (node->right) {
        bst_replace_node_in_parent(node, node->right);
    } else {
	#ifdef DEBUG
	printf("bst_delete_node: bst_replace_node_in_parent(%08X, 0)\n", node);
	#endif
        bst_replace_node_in_parent(node, 0);
    }
}
