#include <zlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
