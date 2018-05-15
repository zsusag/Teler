#define _GNU_SOURCE

#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <zlib.h>

#include "data_structures/hash_map/hash_map.h"
#include "data_structures/tree/tree.h"
#include "util.h"
#include "teler.h"
#include "commit.h"
#include "compression.h"
#include "stream.h"

void traverse_commit(hash_map_t* h, char* object) {
  FILE* fp = NULL;
  open_object_file(&fp, object, "rb");

  // Declare a memstream and open it
  stream_t s;
  open_memstream_safe(&s);

  // Decompress the entire file and write it to the memory stream
  int ret;
  if((ret = inf(fp, s.stream)) != Z_OK) {
    zerr(ret);
  }
  fclose(fp);
  rewind_memstream(&s);

  // Read each line of the file and recursively add objects to the hash map
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

    // Add the current object to the hash map
    hash_map_set(h, next_object, md);

    // If the object was a tree, then recurse. Else, continue
    if(md->type == tree) {
      traverse_commit(h, next_object);
    }
  }
  free(ln);
  close_memstream(&s);
}

void traverse_working_dir(hash_map_t* h, tnode_t* t, char* dir_path) {
  DIR* dir = opendir(dir_path);
  if(dir == NULL) {
    fprintf(stderr, "Unable to open working directory. Exiting...\n");
    exit(EXIT_FAILURE);
  }
  struct dirent* f;
  // Iterate through the list of files within the working directory
  while((f = readdir(dir)) != NULL) {
    if(strcmp(f->d_name, ".") == 0 || strcmp(f->d_name, "..") == 0 || strcmp(f->d_name, ".teler") == 0) {
      continue;
    }
    // Allocate memory for the filename. +2 for the null byte and a '/'.
    size_t filepath_length = strlen(f->d_name) + strlen(dir_path) + 2;
    char* filepath = (char*) malloc(filepath_length);
    // Clear filename of garbage data
    memset(filepath, '\0', filepath_length);

    // Copy in the requested directory_name, append it with a '/' and the found filepath
    strncpy(filepath, dir_path, strlen(dir_path));
    strncat(filepath, "/", 1);
    strncat(filepath, f->d_name, strlen(f->d_name));

    // Call stat in order to retrieve information about the current file
    struct stat f_stat;
    if(stat(filepath, &f_stat) != 0) {
      //perror("Error in retrieving metadata");
      fprintf(stderr, "Error in retrieving %s's metadata\n", filepath);
      exit(EXIT_FAILURE);
    }

    // Allocate space for the filepath to remove the preceeding directory name
    char* filename = (char*) malloc(strlen(f->d_name) + 1);
    // Clear filename of garbage data
    memset(filename, '\0', strlen(f->d_name) + 1);
    strncpy(filename, f->d_name, strlen(f->d_name));

    /* If it is a normal file, hash it, see if it is necessary to make a new object, if so,
       make that new object, and add it to the hash table and tree structure.*/
    if(S_ISDIR(f_stat.st_mode)) {
      // The current file is a directory
   
      // Add the directory to the directory tree
      dirtree_insert(t, NULL);
      tnode_t* child = dirtree_get_latest(t);
      traverse_working_dir(h, child, filepath);
      
      // Calculate hash of the root of the tree by looking at each its children
      dirtree_hash(child, h);

      // Gather the metadata for the directory
      metadata_t* md = (metadata_t*) malloc(sizeof(metadata_t));
      md->perm = f_stat.st_mode;
      md->type = tree;
      md->filename = filename;

      // Retrieve the hash and copy it into a new chunk of memory
      char* hash = (char*) malloc(41);
      strncpy(hash, dirtree_get_hash(child), 41);

      // Add the directory into the hash map
      hash_map_set(h, hash, md);
    } else {
      // Else, the file is a regular file and thus would be a blob

      // Open the file for reading in binary mode
      FILE* fp_input = NULL;
      open_file(&fp_input, filepath, "rb");

      // Declare block buffer for reading data in from fp_input
      unsigned char block[BLOCK_SIZE] = {0};
      size_t block_len = 0;

      // Initialize a SHA1 hash
      SHA_CTX c;
      if(SHA1_Init(&c) == 0) {
        fprintf(stderr, "Failed to initialize a SHA1 hash\n");
        exit(EXIT_FAILURE);
      }

      // Read in the entire file in BLOCK_SIZE chunks, hashing it along the way
      while((block_len = fread(block, 1, BLOCK_SIZE, fp_input)) != 0) {
        // Update the hash with the newly read block
        sha1_update_safe(&c, block, block_len);

        // Reset the block_length to 0
        block_len = 0;
      }
      // Finalize the hash
      unsigned char bin_hash[20];
      if(SHA1_Final(bin_hash, &c) == 0) {
        fprintf(stderr, "Unable to finalize hash\n");
        exit(EXIT_FAILURE);
      }

      // Convert the hash into hexadecimal notation
      char* hash = bin2hex(bin_hash, 20);

      // Look in the hash map to see if the file has changed
      if(!hash_map_contains(h, hash)) {
        // If the map does not contain the file, copy over the file into the
        // shadow directory
        
        // Open the corresponding object file
        FILE* fp_output = NULL;
        open_object_file(&fp_output, hash, "wb");

        // Rewind the input file
        rewind(fp_input);

        // Compress the file into the shadow directory
        int ret;
        if((ret = def(fp_input, fp_output, Z_DEFAULT_COMPRESSION)) != Z_OK) {
          zerr(ret);
        }
        // Close the output file
        fclose(fp_output);

        // Add the file to the hash table
        metadata_t* md = (metadata_t*) malloc(sizeof(metadata_t));
        md->perm = f_stat.st_mode;
        md->type = blob;
        md->filename = filename;
        
        hash_map_set(h, hash, md);
      }
      // Close the input file
      fclose(fp_input);

      // Copy the hash into a different set of memory to prevent double frees
      char* hash_tree = (char*) malloc(41);
      memcpy(hash_tree, hash, 41);
      // Put the blob into the tree
      dirtree_insert(t, hash_tree);
    }
  }
}

bool populate_hash_table(hash_map_t* h) {
  // Get the pathname for heads directory
  char* commit = get_latest_commit("master");

  FILE* commit_fp = NULL;
  if((commit_fp = fopen(commit, "r")) != NULL) {
    // If this is not the first commit, read in the latest commit
    char* commit_hex = NULL;
    if(readline(&commit_hex, commit_fp) == 0) {
      fprintf(stderr, "%s is empty. Something went wrong...\n", commit);
      exit(EXIT_FAILURE);
    }
    // Close the file
    fclose(commit_fp);
    
    // Open commit file
    open_object_file(&commit_fp, commit_hex, "rb");
    
    // Initialize stream to store commit file within it
    stream_t s;
    open_memstream_safe(&s);

    // Decompress the commit file and store it into the stream
    int ret;
    if((ret = inf(commit_fp, s.stream)) != Z_OK) {
        zerr(ret);
    }
    fclose(commit_fp);

    // Rewind the stream to the beginning
    rewind_memstream(&s);

    // Read in the first line of the commit containing the root of the tree
    free(commit_hex);
    if(readline(&commit_hex, s.stream) == 0) {
      fprintf(stderr, "Commit is empty\n");
      exit(EXIT_FAILURE);
    }
    close_memstream(&s);
    char* commit_hex_hd = commit_hex; // NOTE: hack

    strtok(commit_hex, " ");
    commit_hex = strtok(NULL, " ");

    // Add the root of the tree to the hash map
    metadata_t* cur_md = (metadata_t*) malloc(sizeof(metadata_t));
    cur_md->perm = S_IFDIR;
    cur_md->filename = ".";
    cur_md->type = tree;
    hash_map_set(h, commit_hex, cur_md);

    // Recursively add the entire commit structure to the hash map
    traverse_commit(h, commit_hex);
    free(commit_hex_hd);
    return false;
  }
  return true;
}

void push() {
  // Allocate memory for a hash map to store all of the latest commits objects
  hash_map_t h;
  hash_map_init(&h);

  // Populate the hash table with the latest commit, if there is one
  bool initial = populate_hash_table(&h);

  tnode_t t;
  dirtree_init(&t);
  t.hash = NULL;

  // Traverse the working dir to collect all changes
  traverse_working_dir(&h, &t, ".");
  dirtree_hash(&t, &h);

  // Generate the commit file and write it
  commit_t c;
  if(!initial) {
    // Retrieve the latest commit
    char* latest_commit = get_latest_commit("master");

    // Open the head of the current branch
    FILE* fp;
    open_file(&fp, latest_commit, "r");

    // Read in the latest commit message
    char* parent;
    if(readline(&parent, fp) == 0) {
      fprintf(stderr, "Unable to read in latest commit\n");
      exit(EXIT_FAILURE);
    }
    // Generate the commit
    mk_commit(&c, dirtree_get_hash(&t), parent);
  } else {
    // Generate the commit
    mk_commit(&c, dirtree_get_hash(&t), NULL);
  }
  
  // Write the commit to the shadow directory
  write_commit(&c);
  destroy_commit(&c);
  
  // Traverse the directory tree, writing the information to the directory tree
  dirtree_traverse(&t, &h);

  // Destroy the hash map
  hash_map_destroy(&h);

  // Destroy the directory tree
  dirtree_destroy(&t);
}
