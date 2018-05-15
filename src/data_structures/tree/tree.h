#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <openssl/sha.h>

#include "../hash_map/hash_map.h"
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
  char* hash;
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
void dirtree_init(tnode_t* t);

// Destroy a directory tree
void dirtree_destroy(tnode_t* t);

// Insert a child into the directory tree
void dirtree_insert(tnode_t* t, char* child_hash);

// Get the hash of the current root of the directory tree
char* dirtree_get_hash(tnode_t* t);

// Get the latest added child from a directory tree
tnode_t* dirtree_get_latest(tnode_t* t);

// Traverse a directory tree, printing it out into the shadow directory
void dirtree_traverse(tnode_t* t, hash_map_t* h);

// Hash a interior node by hashing all of the children together
void dirtree_hash(tnode_t* t, hash_map_t* h);

// Traverse the tree, writing the tree to the working directory
void dirtree_reconstruct_working_dir(tnode_t* t, hash_map_t* h, char* path);

// Initialize a child list
void clist_init(clist_t* c);

// Destroy a child list
void clist_destroy(clist_t* c);

// Insert an element into a child list
void clist_insert(clist_t* c, char* child_hash);

// Get the head of a child list
cnode_t* clist_get_head(clist_t* c);

// Traverse each child in a child list and hash them together
void clist_hash(clist_t* c, hash_map_t*h, SHA_CTX* sha);

// Traverse a child list
void clist_traverse(clist_t* c, hash_map_t* h, FILE* fp);

// Traverse a child list, writing the list to the working directory
void clist_reconstruct_working_dir(clist_t* c, hash_map_t* h, char* path);
#endif
