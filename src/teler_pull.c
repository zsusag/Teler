#include <stdio.h>

#include "data_structures/hash_map/hash_map.h"
#include "data_structures/tree/tree.h"
#include "teler.h"
#include "commit.h"
#include "revert.h"
#include "util.h"

void pull() {
  // Get the information about the latest commit, if there is one
  commit_t* commit;
  if((commit = reconstruct_commit(NULL)) == NULL) {
    fprintf(stderr, "No commits are present within the repository.\n");
    exit(EXIT_FAILURE);
  }
  
  // Allocate memory for a hash map to store all of the latest commit objects
  hash_map_t h;
  hash_map_init(&h);

  // Add the root of the directory to the hash map
  metadata_t* md = (metadata_t*) malloc(sizeof(metadata_t));
  md->perm = RWX_OWNER_PERM;
  md->filename = ".";
  md->type = tree;
  
  hash_map_set(&h, commit->tree, md);

  // Allocate memory for a directory tree
  tnode_t t;
  dirtree_init(&t);
  t.hash = commit->tree;
  // Populate the hash map and build the directory tree
  build_tree(&h, &t, t.hash);

  // Clear the current working directory
  clear_working_dir(".");

  // Reconstruct the working directory
  dirtree_reconstruct_working_dir(&t, &h, ".");

  // Clean up
  hash_map_destroy(&h);
  dirtree_destroy(&t);
  destroy_commit(commit);
  free(commit);
}
