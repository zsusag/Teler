#include <stdlib.h>

#include "tree.h"

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

  // Initialize the child subtree with the hash
  dirtree_init(child, child_hash);

  // Put the new node at the front of the children list
  new_node->next = c->head;
  c->head = new_node;
}


