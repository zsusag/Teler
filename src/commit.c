#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>

#include "commit.h"
#include "util.h"
#include "stream.h"
#include "compression.h"

void hash_commit(commit_t* c) {
  // Initialize the SHA1 hash
  SHA_CTX sha;
  if(SHA1_Init(&sha) == 0) {
    fprintf(stderr, "Unable to initialize SHA1 hash\n");
    exit(EXIT_FAILURE);
  }

  // Update the hash with all the information of the hash
  sha1_update_safe(&sha, c->tree, strlen(c->tree));
  sha1_update_safe(&sha, c->author, strlen(c->author));
  sha1_update_safe(&sha, c->timestamp, strlen(c->timestamp));
  sha1_update_safe(&sha, c->msg, strlen(c->msg));
  if(c->parent != NULL) {
    sha1_update_safe(&sha, c->author, strlen(c->author));
  }

  // Finalize the SHA1 hash
  unsigned char hash_bin[20];
  if(SHA1_Final(hash_bin, &sha) == 0) {
    fprintf(stderr, "Unable to finalize SHA1 hash\n");
    exit(EXIT_FAILURE);
  }

  // Convert the hash to hexadecimal notation
  c->hash = bin2hex(hash_bin, 20);
}

void mk_commit(commit_t* c, char* tree, char* parent) {
  // Set the tree and parent fields
  c->tree = tree;
  c->parent = parent;

  // Retrieve the author's name and email from the config file
  char* config = get_shadow_dir();
  // Allocate enough space for "config" and the null byte
  config = (char*) realloc(config, strlen(config) + 7);
  strncat(config, "config", 6);

  // Open the configuration file
  FILE* config_fp;
  open_file(&config_fp, config, "r");

  // Read in the file and get the author
  char* author;
  if(readline(&author, config_fp) == 0) {
    fprintf(stderr, "Unable to retrieve author from configuration file\n");
    exit(EXIT_FAILURE);
  }
  // Close the configuration file and set the author in the commit
  fclose(config_fp);
  c->author = author;

  // Generate the timestamp
  c->timestamp = get_timestamp();

  // Prompt the user to enter a commit message and store it into the commit
  printf("Please enter a commit message followed by <ENTER>: ");
  readline(&c->msg, stdin);

  // Generate the hash of the commit
  hash_commit(c);
}

void destroy_commit(commit_t* c) {
  // Free all allocated memory that the user didn't supply
  free(c->author);
  free(c->timestamp);
  free(c->msg);
  free(c->hash);
}

void write_commit(commit_t* c) {
  // Open the object file which will contain the commit
  FILE* fp;
  open_object_file(&fp, c->hash, "wb");

  // Open a memory stream
  stream_t s;
  open_memstream_safe(&s);

  // Write the commit information to the memory stream
  fprintf(s.stream, "tree %s\nparent %s\nauthor %s %s\n%s", c->tree,
          c->parent, c->author, c->timestamp, c->msg);
  rewind_memstream(&s);

  // Compress the commit file and write it to the shadow directory
  int ret;
  if((ret = def(s.stream, fp, Z_DEFAULT_COMPRESSION)) != Z_OK) {
    zerr(ret);
  }
  close_memstream(&s);
  fclose(fp);

  // Open up refs/heads/master
  char* branch_head = get_latest_commit("master");
  open_file(&fp, branch_head, "w");

  // Write the new hash to the refs/heads file
  if(fwrite(c->hash, 1, 40, fp) != 40) {
    fprintf(stderr, "Unable to fully write new refs/heads file\n");
    exit(EXIT_FAILURE);
  }
}
