#define _GNU_SOURCE
#include <unistd.h>
#include <zlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "data_structures/hash_map/hash_map.h"
#include "data_structures/tree/tree.h"
#include "commit.h"
#include "stream.h"
#include "teler.h"
#include "util.h"
#include "compression.h"

commit_t* reconstruct_commit(char* given_commit) {
  // Initialize a boolean to free the commit if allocated
  bool mallocd = false;

  char* commit = given_commit;

  // If commit is NULL, then use the latest commit
  if(commit == NULL) {
    char* filepath = get_latest_commit("master");

    FILE* fp;
    if((fp = fopen(filepath, "r")) != NULL) {
      mallocd = true;
      // If the commit exists, then read it in
      if(readline(&commit, fp) == 0) {
        fprintf(stderr, "%s is empty. Something went wrong...\n", filepath);
        exit(EXIT_FAILURE);
      }
      // Close the file
      fclose(fp);
    } else {
      free(filepath);
      return NULL;
    }
    free(filepath);
  }
  if(!mallocd) {
    commit = (char*) malloc(strlen(given_commit));
    memcpy(commit, given_commit, strlen(given_commit));
  }

  // Open the commit file
  FILE* fp;
  open_object_file(&fp, commit, "rb");

  // Initialize a stream to store the inflated commit file within
  stream_t s;
  open_memstream_safe(&s);

  // Inflate the commit file and store it within the stream
  int ret;
  if((ret = inf(fp, s.stream)) != Z_OK) {
    zerr(ret);
  }
  // Close the input file
  fclose(fp);

  // Rewind the stream to the beginning
  rewind_memstream(&s);

  // Allocate space for a commit
  commit_t* c = (commit_t*) malloc(sizeof(commit_t));

  // Parse the information within the buffer.
  sscanf(s.buf, "tree %ms\nparent %ms\nauthor %ms\n%ms\n%ms", &(c->tree), &(c->parent),
         &(c->author), &(c->timestamp), &(c->msg));
  c->hash = commit;

  // Close the memstream
  close_memstream(&s);
  return c;
}

void build_tree(hash_map_t* h, tnode_t* t, char* commit) {
  // Open the object file containing object (tree)
  FILE* fp;
  open_object_file(&fp, commit, "rb");

  // Declare a memstream and open it
  stream_t s;
  open_memstream_safe(&s);

  // Decompress the entire file and write it to the memory stream
  int ret;
  if((ret = inf(fp, s.stream)) != Z_OK) {
    zerr(ret);
  }
  // Close the input file and rewind the memstream
  fclose(fp);
  rewind_memstream(&s);

  // Read in each line of the file and recursively add objects to the hash map
  char* ln = NULL;
  while(readline(&ln, s.stream) != -1) {
    metadata_t* md = (metadata_t*) malloc(sizeof(metadata_t));

    // Parse the line in the tree
    char* tmp = NULL;
    tmp = strtok(ln, " ");
    md->perm = (mode_t) strtoul(tmp, NULL, 8);
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
    memset(md->filename, '\0', strlen(tmp) + 1);
    memcpy(md->filename, tmp, strlen(tmp));

    // Make a copy of the next object for the tree
    char* next_object_tree = malloc(strlen(next_object) + 1);
    memset(next_object_tree, '\0', strlen(next_object) + 1);
    memcpy(next_object_tree, next_object, strlen(next_object));

    // Add the current object to the hash map
    hash_map_set(h, next_object, md);

    // Add the object to the directory tree
    dirtree_insert(t, next_object_tree);

    // If the object was a tree, then recurse. Else, continue
    if(md->type == tree) {
      build_tree(h, dirtree_get_latest(t), next_object);
    }
  }
  free(ln);
  close_memstream(&s);
}

void clear_working_dir(char* path) {
  // Open the directory
  DIR* dir = opendir(path);
  if(dir == NULL) {
    fprintf(stderr, "Unable to open working directory. Exiting...\n");
    exit(EXIT_FAILURE);
  }
  struct dirent* f;
  // Iterate through the list of files within the working directory, deleting them
  while((f = readdir(dir)) != NULL) {
    // If the current file is '.', '..', or '.teler', skip it
    if(strcmp(f->d_name, ".") == 0 || strcmp(f->d_name, "..") == 0 || strcmp(f->d_name, ".teler") == 0) {
      continue;
    }
    // Create the full file path
    char* f_path = (char*) construct_filepath(path, f->d_name);
    
    // If the file is a directory then recursively remove elements from it
    if(f->d_type == DT_DIR) {
      clear_working_dir(f_path);
    }

    // Remove the file
    if(remove(f_path) != 0) {
      perror("Unable to remove file");
      exit(EXIT_FAILURE);
    }

    // Clean up
    free(f_path);
  }
}
