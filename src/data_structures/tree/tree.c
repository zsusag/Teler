#include <openssl/sha.h>
#include <stdio.h>
#include <zlib.h>
#include <string.h>

#include "tree.h"
#include "../hash_map/hash_map.h"
#include "../../util.h"
#include "../../stream.h"
#include "../../compression.h"

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
  // Initialize a memory stream
  stream_t s;
  open_memstream_safe(&s);

  // Traverse the list of children of the current node in the tree
  clist_traverse(t->children, h, s.stream);

  // Rewind the stream
  rewind_memstream(&s);

  // Open a file to the corresponding file within the shadow directory
  FILE* fp;
  open_object_file(&fp, t->hash, "wb");

  // Compress the tree object to the directory
  int ret;
  if((ret = def(s.stream, fp, Z_DEFAULT_COMPRESSION)) != Z_OK) {
    zerr(ret);
  }
  // Close the memory stream and file
  close_memstream(&s);
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

void dirtree_reconstruct_working_dir(tnode_t* t, hash_map_t* h, char* path) {

  // Retrieve the information of the current root of the tree from the hash map
  metadata_t* root_md = hash_map_get(h, t->hash);
  if(root_md == NULL) {
    fprintf(stderr, "Unable to find %s in hash map\n", t->hash);
    exit(EXIT_FAILURE);
  }
  // Construct the directory, if it doesn't already exist
  struct stat root_st = {0};
  if(stat(path, &root_st) == -1) {
    // The directory doesn't exist
    if(mkdir(path, root_md->perm) != 0) {
      perror("Unable to create directory");
      exit(EXIT_FAILURE);
    }
  }

  // Traverse each child and create the files
  clist_reconstruct_working_dir(t->children, h, path);
}
