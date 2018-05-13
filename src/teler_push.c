#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "data_structures/hash_map/hash_map.h"
#include "util.h"
#include "teler.h"

void traverse_commit(hash_map_t* h, char* object) {
  FILE* fp = NULL;
  open_object_file(&fp, object, "r");

  // Read each line of the file and recursively add objects to the hash map
  char* ln = NULL;
  while(readline(&ln, fp) != -1) {
    metadata_t* md = (metadata_t*) malloc(sizeof(metadata_t));
    // Parse the line in the tree
    char* tmp = NULL;
    tmp = strtok(ln, " ");
    md->perm = (mode_t) tmp;
    tmp = strtok(NULL, " ");
    md->type = convert_to_type(tmp);

    // Retrieve the hash of the next object
    tmp = strtok(NULL, " ");
    char* next_object = malloc(strlen(tmp) + 1);
    strncpy(next_object, tmp, strlen(tmp));

    // Get the human readable filename
    tmp = strtok(NULL, " ");

    // Copy the filename into the metadata struct
    md->filename = (char*) malloc(strlen(tmp) + 1);
    strncpy(md->filename, tmp, strlen(tmp));

    // Add the current object to the hash map
    hash_map_set(h, next_object, md);

    // If the object was a tree, then recurse. Else, continue
    if(md->type == tree) {
      traverse_commit(h, next_object);
    }
  }
  free(ln);
}

void populate_hash_table(hash_map_t* h) {
  // Get the pathname for heads directory
  char* commit = get_latest_commit("master");

  FILE* commit_fp = NULL;
  if((commit_fp = fopen(commit, "r")) != NULL) {
    // If this is not the first commit, read in the latest commit
    char* commit_hex = NULL;
    if(readline(&commit_hex, commit_fp) == -1) {
      fprintf(stderr, "%s is empty. Something went wrong...\n", commit);
      exit(EXIT_FAILURE);
    }

    // Add the root of the tree to the hash map
    metadata_t* cur_md = (metadata_t*) malloc(sizeof(metadata_t));
    cur_md->perm = S_IFDIR;
    cur_md->filename = ".";
    cur_md->type = tree;
    hash_map_set(h, commit_hex, cur_md);

    // Recursively add the entire commit structure to the hash map
    traverse_commit(h, commit_hex);
  }
  // TODO: Traverse working directory
  
}



void push() {
  /* Steps:
     1) Read in the latest commit and reconstruct its shadow structure getting all of the blobs
     2) For each file within the working tree, hash it, and check to see if the hash is in the shadow tree.
       - If so, the file is unchanged. We copy over the shadow version into the new commit tree
       - If not, then the file is changed, new, or deleted. Either way, we disregard what was in the shadow tree
         and add only the working tree version, if any. 
   */

  /* Two cases:
     1) Initial commit, no refs/heads/master
     2) Not initial commit*/
  // Allocate memory for a hash map to store all of the latest commits objects
  hash_map_t h;
  hash_map_init(&h);

  

  // Destroy the hash map
  hash_map_destroy(&h);
}
