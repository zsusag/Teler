#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
// Define an initial capacity for the hash table
#define INIT_CAPACITY 32

// This makes the header file work for both C and C++
#ifdef __cplusplus
extern "C" {
#endif

  /**
   * A struct to hold all of metadata for a given file
   */
  typedef struct metadata {
    mode_t perm; // Permission of file
    char* filename; // The human-readable filename
    char* type; // Type of the file
  } metadata_t;
  /**
   * A node structure to hold key, permissions, filename, and type
   */
  typedef struct node {
    const unsigned char* key; // The hash of the blob
    metadata_t* val;
    struct node* next;
  } node_t;

  /**
   * A list structure to house the contents of a linked list,
   * to serve as buckets within the associative array/hash table.
   */
  typedef struct list{
    node_t* head;
  } list_t;

  /**
   * A hash map structure to house metadata information for the
   * hash map along with the actual hash table made up of
   * a certain number of buckets which will be resized as necessary.
   */
  typedef struct hash_map {
    list_t* table;
    size_t capacity;
    size_t size;
    size_t load;
    unsigned int salt;
  } hash_map_t;

  // Initialize a hash map
  void hash_map_init(hash_map_t* hash_map);

  // Destroy a hash map
  void hash_map_destroy(hash_map_t* hash_map);

  // Set a value in a hash map
  void hash_map_set(hash_map_t* hash_map, const unsigned char* key, metadata_t* value);

  // Check if a hash map contains a key
  bool hash_map_contains(hash_map_t* hash_map, const unsigned char* key);

  // Get a value in a hash map
  metadata_t* hash_map_get(hash_map_t* hash_map, const unsigned char* key);

  // Remove a value from a hash map
  void hash_map_remove(hash_map_t* hash_map, const unsigned char* key);

  // Grow a hash map to double its size
  void hash_map_grow(hash_map_t* hash_map);

  // Initialize a linked list to be used for the buckets for the hash map
  void list_init(list_t* l);

  /* Insert a key,value pair into the list, updating the value
     if a node with the given key is already present. Returns
     true if a new node was created*/
  bool list_insert(list_t* l, const unsigned char* key, metadata_t* val);

  /* Retrieve a value from the list which corresponds to a given key.
     Returns NULL if a value associated with the given key is not found. */
  metadata_t* list_lookup(list_t* l, const unsigned char* key);

  // Remove a node from the list which contains the given key.
  bool list_remove(list_t* l, const unsigned char* key);

  // Destroy an entire linked list (bucket)
  void list_destroy(list_t* l);

  // This makes the header file work for both C and C++
#ifdef __cplusplus
}
#endif

#endif
