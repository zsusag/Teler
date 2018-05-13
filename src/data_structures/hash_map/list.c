#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hash_map.h"

/**
 * This function will initialize a linked list.
 *
 * \param l               A pointer to a linked list with preallocated memory
 */
void list_init(list_t* l) {
  l->head = NULL;
}

/**
 * This function will insert a new key,value pair into a list, l,
 * given that an entry with the given key doesn't already exist. If an entry does
 * exist, then the value is replaced with the new value.
 */
bool list_insert(list_t* l, const char* key, metadata_t* val) {
  /* Search to see if the key is already in
     the bucket. If it is, set its value to val
     and return. */
  node_t* cur = l->head;
  while(cur != NULL) {
    if(strcmp(cur->key,key) == 0) {
      cur->val = val;
      return false;
    }
    cur = cur->next;
  }
  /* If not, malloc a new node and place it on the front
     of the linked list. */
  node_t* new_node = (node_t*) malloc(sizeof(node_t));
  if(new_node == NULL) {
    perror("malloc failed in acquiring memory on the heap");
    exit(EXIT_FAILURE);
  }
  new_node->key = key;
  new_node->val = val;
  new_node->next = l->head;
  l->head = new_node;
  return true;
}

/**
 * This function will find a key,value pair associated with the given key
 * in a list l. If an entry was not found, -1 is returned. Else, the value
 * associated with the key is returned.
 *
 * \param l               A pointer to a linked list
 * \param key             The search key
 *
 * \returns               The value associated with the given key. If such a
 *                        value was not found, -1 is returned.
 */
metadata_t* list_lookup(list_t* l, const char* key) {
  node_t* cur = l->head;
  /* Traverse the list until either the end of the list is hit or
     the given key was found in the list. */
  while(cur != NULL) {
    // If the key was found, return the value.
    if(strcmp(cur->key,key) == 0) {
      metadata_t* ret = cur->val;
      return ret;
    }
    cur = cur->next;
  }
  // A value associated with the given key was not found, so return NULL.
  return NULL;
}

/**
 * This function will remove a key,value pair from a list, l, if such
 * an entry was found with the given key. If not, then do nothing.
 *
 * \param l               A pointer to a linked list
 * \param key             The search key
 *p
 * \returns               True if the key was found in the table, and thus
 *                        removed or false if the key wasn't found.
 */
bool list_remove(list_t* l, const char* key) {
  node_t* cur = l->head;
  node_t* prev = NULL;

  /* If head is the node to be deleted then
     set the head equal to the next element in the list,
     free the deleted element, unlock, and return. */
  if(cur != NULL && strcmp(cur->key,key) == 0) {
    l->head = cur->next;
    free(cur);
    return true;
  }

  /* Iterate through the list until we either
     reach the end of the list or we found
     the node to be removed. */
  while(cur != NULL && strcmp(cur->key,key) != 0) {
    prev = cur;
    cur = cur->next;
  }

  /* If we hit the end of the list then
     simply unlock and return as a node with the
     supplied key is not in the list. */
  if(cur == NULL) {
    return false;
  }

  /* Else, free the to-be-deleted node, unlock
     and return. */
  prev->next = cur->next;
  free(cur);
  return true;
}

/**
 * This function will destroy an entire linked list, requiring
 * the list, l, to be reinitalized if it is going to be used again.
 *
 * \param l               A pointer to a linked list
 */
void list_destroy(list_t* l) {
  node_t* cur = l->head;
  while(cur != NULL) {
    node_t* tmp = cur;
    cur = cur->next;
    free(tmp);
  }
  l->head = NULL;
  return;
}
