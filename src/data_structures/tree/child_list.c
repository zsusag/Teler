#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>

#include "../../util.h"
#include "tree.h"
#include "../../compression.h"

// Initialize a child list
void clist_init(clist_t* c) {
  c->head = NULL;
}

// Destroy a child list
void clist_destroy(clist_t* c) {
  // Iterate through the clist, destroying all of the tnodes along the way
  cnode_t* cur = c->head;
  while(cur != NULL) {
    // Recursively destroy the subtree
    dirtree_destroy(cur->child);

    // Move the cursor to the next element
    cnode_t* tmp = cur;
    cur = cur->next;

    // Free the subtree along with the cnode
    free(tmp->child);
    free(tmp);
  }
}

// Insert an element into a child list
void clist_insert(clist_t* c, char* child_hash) {
  // Allocate memory for a cnode and a tnode
  cnode_t* new_node = (cnode_t*) malloc(sizeof(cnode_t));
  tnode_t* child = (tnode_t*) malloc(sizeof(tnode_t));

  new_node->child = child;

  // Initialize the child subtree with the hash
  dirtree_init(child);

  child->hash = child_hash;
  // Put the new node at the front of the children list
  new_node->next = c->head;
  c->head = new_node;
}

// Get the head of a child list
cnode_t* clist_get_head(clist_t* c) {
  return c->head;
}

// Traverse each child in a child list and hash them together
void clist_hash(clist_t* c, hash_map_t* h, SHA_CTX* sha) {
  cnode_t* cur = c->head;
  while(cur != NULL) {
    char* hash = dirtree_get_hash(cur->child);
    metadata_t* md = hash_map_get(h, hash);
    if(md == NULL) {
      fprintf(stderr, "No hash found within hashmap while hashing tree\n");
      exit(EXIT_FAILURE);
    }
    // Update the sha hash with the child's data
    sha1_update_safe(sha, &md->perm, sizeof(md->perm));
    sha1_update_safe(sha, &md->type, sizeof(md->type));
    sha1_update_safe(sha, hash, 40); // 20 bytes long = 40 hex chars
    sha1_update_safe(sha, md->filename, strlen(md->filename));

    cur = cur->next;
  }
}

// Traverse a child list
void clist_traverse(clist_t* c, hash_map_t* h, FILE* fp) {
  cnode_t* cur = c->head;
  while(cur != NULL) {
    // Retrieve the metadata information for the given file
    metadata_t* md = hash_map_get(h, dirtree_get_hash(cur->child));

    // Print out the metadata into the tree file
    fprintf(fp, "%o %s %s %s\n", md->perm, enum2string(md->type), 
            dirtree_get_hash(cur->child), md->filename);

    // If the child is a tree itself, recursively print out
    if(md->type == tree) {
      dirtree_traverse(cur->child, h);
    }

    // Advance cur
    cur = cur->next;
  }
}

void clist_reconstruct_working_dir(clist_t* c, hash_map_t* h, char* path) {
  cnode_t* cur = c->head;
  while(cur != NULL) {
    // Retreive the metadata information for the given file
    metadata_t* md = hash_map_get(h, dirtree_get_hash(cur->child));

    // Construct the filepath
    char* filepath = construct_filepath(path, md->filename);
    
    // If the current child is a blob, decompress it and place it in the working dir
    if(md->type == blob) {
      // Open the file in the working for writing in binary mode
      FILE* output;
      open_file(&output, filepath, "wb");

      // Open the file in the shadow directory for reading in binary mode
      FILE* input;
      open_object_file(&input, dirtree_get_hash(cur->child), "rb");

      // Decompress the file and write it to the file pointed to by output
      int ret;
      if((ret = inf(input, output)) != Z_OK) {
        zerr(ret);
      }

      // Change the permissions to the ones that are stored
      if(chmod(filepath, md->perm) != 0) {
        perror("Unable to change permissions of file");
        exit(EXIT_FAILURE);
      }
    } else {
      // Else, it is a tree, and so recurse down
      dirtree_reconstruct_working_dir(cur->child, h, filepath);
    }
    // Advance the cursor
    cur = cur->next;

    // Free filepath
    free(filepath);
  }
}
