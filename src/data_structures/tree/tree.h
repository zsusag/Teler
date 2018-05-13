#ifndef TREE_H
#define TREE_H

/*
  This data structure is to preserve the hierarchical nature of directories
  by creating a parent/child relationship between objects (i.e. trees and blobs)

  We encode this relationship using a n-tree where interior nodes are tree
  objects and leaves are blob objects.
*/

typedef struct tnode tnode_t;
typedef struct cnode cnode_t;
typedef struct clist clist_t;

struct tnode {
  unsigned char* hash;
  clist_t* children;
};

struct cnode {
  tnode_t* child;
  cnode_t* next;
};

struct clist {
  cnode_t* head;
};

// Initialize a directory tree
void dirtree_init(tnode_t* t, unsigned char* hash);

// Destroy a directory tree
void dirtree_destroy(tnode_t* t);

// Insert a child into the directory tree
void dirtree_insert(tnode_t* t, unsigned char* child_hash);

// Traverse a directory tree, printing it out into the shadow directory
void dirtree_traverse(tnode_t* t);

// Initialize a child list
void clist_init(clist_t* c);

// Destroy a child list
void clist_destroy(clist_t* c);

// Insert an element into a child list
void clist_insert(clist_t* c, unsigned char* child_hash);



#endif
