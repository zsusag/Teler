#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#include "hash_map.h"
/**
 * This hash function will hash a given key, starting with a salt
 * and return an index into a hash table, bounded by the capacity.
 *
 * Citation: Per-Ake Larson of Microsoft Research: doi 10.1145/42404.42410
 *
 * \param key          The key to be hashed
 * \param salt         A randomized salt associated with the hash table
 * \param capacity     The total number of buckets available in the table
 *
 * \returns hash       The hash of the key which is an index into the table
 */
size_t hash_fn(const char* key, unsigned int salt, size_t capacity){
  unsigned int h = salt;
  while(*key){
    h = h*101 + (unsigned int)*key++;
  }
  return h % capacity;
}

/**
 * This function will initialize a hash map.
 *
 * \param              A hash map which already has memory allocated
 */
void hash_map_init(hash_map_t* hash_map) {
  hash_map->capacity = INIT_CAPACITY;
  hash_map->size = 0;
  // Set load to be 75% of the capacity.
  hash_map->load = (3*INIT_CAPACITY)/4;

  // Generate a random salt
  srand(time(NULL)* getpid());
  hash_map->salt = rand();

  hash_map->table = (list_t*) malloc(sizeof(list_t) * INIT_CAPACITY);
  if(hash_map->table == NULL) {
    perror("malloc was unable to acquire enough memory");
    exit(EXIT_FAILURE);
  }
  // Initialize all of the buckets
  for(int i = 0; i < INIT_CAPACITY; i++) {
    list_init(&hash_map->table[i]);
  }
}

/**
 * This function will destroy an entire hash map, freeing the memory
 * as it destroys.
 *
 * \param hash_map            A hash map
 */
void hash_map_destroy(hash_map_t* hash_map) {
  // Destroy all of the buckets
  for(size_t i = 0; i < hash_map->capacity; i++) {
    list_destroy(&hash_map->table[i]);
  }
  free(hash_map->table);
  return;
}

/**
 * This function adds or updates an entry in hash_map. If there is already
 * an entry for key, then its value will be updated. If no entry exists
 * for key, a new one is added.
 *
 * \param hash_map        A hash map 
 * \param key             The key of the entry
 * \param value           The value associated with the key in the entry
 */
void hash_map_set(hash_map_t* hash_map, const char* key, metadata_t* val) {
  size_t hash = hash_fn(key, hash_map->salt, hash_map->capacity);
  // If the key was not in the table then increment the size
  if(list_insert(&hash_map->table[hash], key, val)) {
    hash_map->size++;
    // If the newly added element overloads the hash map, then grow the map.
    if(hash_map->size >= hash_map->load) {
      hash_map_grow(hash_map);
    }
  }
  return;
}

/**
 * This function will return true if the hash map contains a value
 * with the given key.
 *
 * \param hash_map        A hash map
 * \param key             A key to search in the hash map for
 *
 * \returns bool          True if a value associated with the key is found;
 *                        false if not.
 */
bool hash_map_contains(hash_map_t* hash_map, const char* key) {
  size_t hash = hash_fn(key, hash_map->salt, hash_map->capacity);
  return list_lookup(&hash_map->table[hash], key) != NULL;
}

/**
 * This function looks up a key in hash_map. If there is an entry for key,
 * the last value associated with this key is returned. If no matching
 * entry exists, the function returns -1.
 *
 * \param hash_map        A hash map
 * \param key             A key to retrieve the value from
 *
 * \returns val           The value associated with the given key. -1
 *                        if a value was not found for the given key.
 */
metadata_t* hash_map_get(hash_map_t* hash_map, const char* key) {
  size_t hash = hash_fn(key, hash_map->salt, hash_map->capacity);
  return list_lookup(&hash_map->table[hash], key);
}

/**
 * This function removes any entry associated with key. If there is no
 * entry for key, this function does nothing. After removing a key from
 * the hash map, hash_map_contains should return false for that key, even if
 * it was set multiple times.
 *
 * \param hash_map        A hash map
 * \param key             A key to remove from the table
 */
void hash_map_remove(hash_map_t* hash_map, const char* key) {
  size_t hash = hash_fn(key, hash_map->salt, hash_map->capacity);
  // If an entry associated with the given key was found, decrement the size
  if(list_remove(&hash_map->table[hash], key)) {
    hash_map->size--;
  }
  return;
}

/**
 * This function will double the capacity of a hash map.
 *
 * \param hash_map        A hash map
 */
void hash_map_grow(hash_map_t* hash_map) {
  // Retrieve the old contents of the table
  list_t* old_table = hash_map->table;
  size_t old_capacity = hash_map->capacity;

  // Double the capacity of the table and represent changes within hash_map
  hash_map->capacity *= 2;
  hash_map->table = (list_t*) malloc(sizeof(list_t) * hash_map->capacity);
  hash_map->size = 0;
  hash_map->load = (3 * hash_map->capacity) / 4;

  /* Iterate over the old table and rehash and insert the elements into
     the new table */
  node_t* cur = NULL;
  for(size_t i = 0; i < old_capacity; i++) {
    cur = old_table[i].head;
    while(cur != NULL) {
      hash_map_set(hash_map, cur->key, cur->val);
      cur = cur->next;
    }
    list_destroy(&old_table[i]);
  }
}
