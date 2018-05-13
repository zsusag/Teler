

#include "tree.h"


// Initialize a directory tree
void dirtree_init(tnode_t* t, unsigned char* hash) {
  t->hash = hash;
  clist_init(t->children);
}

// Destroy a directory tree
void dirtree_destroy(tnode_t* t) {
  clist_destroy(t->children);
}

// Insert a child into the directory tree
void dirtree_insert(tnode_t* t, unsigned char* child_hash) {
  clist_insert(t->children, child_hash);
}

// Traverse a directory tree, printing it out into the shadow directory
void dirtree_traverse(tnode_t* t) {
  // TODO: Implement traverse
  return;
}
