#include <openssl/sha.h>
#include <stdio.h>

#include "tree.h"
#include "../hash_map/hash_map.h"
#include "../../util.h"

// Initialize a directory tree
void dirtree_init(tnode_t* t) {
  t->children = (clist_t*) malloc(sizeof(clist_t));
  clist_init(t->children);
}

// Destroy a directory tree
void dirtree_destroy(tnode_t* t) {
  clist_destroy(t->children);
  free(t->children);
}

// Insert a child into the directory tree
void dirtree_insert(tnode_t* t, char* child_hash) {
  clist_insert(t->children, child_hash);
}

// Get the latest added child from a directory tree
tnode_t* dirtree_get_latest(tnode_t* t) {
  return clist_get_head(t->children)->child;
}

// Traverse a directory tree, printing it out into the shadow directory
void dirtree_traverse(tnode_t* t, hash_map_t* h) {
  FILE* fp;
  open_object_file(&fp, t->hash, "w");

  clist_traverse(t->children, h, fp);
  fclose(fp);
}

// Hash a interior node by hashing all of the children together
void dirtree_hash(tnode_t* t, hash_map_t* h) {
  SHA_CTX c;
  if(SHA1_Init(&c) == 0) {
    fprintf(stderr, "Could not initialize SHA_CTX\n");
    exit(EXIT_FAILURE);
  }
  // Iterate through the child list and hash the data of all of the children
  clist_hash(t->children, h, &c);

  // Allocate enough memory to hold 20 bytes for the hash
  unsigned char* hash = (unsigned char*) malloc(20);
  if(SHA1_Final(hash, &c) == 0) {
    fprintf(stderr, "Failed to finalize hash\n");
    exit(EXIT_FAILURE);
  }
  // Convert the hash into hexidecimal notation
  t->hash = bin2hex(hash, 20);
}

// Get the hash of the current root of the directory tree
char* dirtree_get_hash(tnode_t* t) {
  return t->hash;
}
