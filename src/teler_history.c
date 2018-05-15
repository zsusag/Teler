#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include <time.h>
#include <stdio.h>
#include <string.h>

#include "data_structures/hash_map/hash_map.h"
#include "data_structures/tree/tree.h"
#include "commit.h"
#include "revert.h"

void history() {
  // Retrieve the commit information for the latest commit
  commit_t* commit = reconstruct_commit(NULL);

  bool first = true;
  // Check to see if there was a head commit
  if(commit == NULL) {
    printf("No history to show. Make some commits!\n");
    exit(EXIT_SUCCESS);
  }

  // Traverse backwards through the history of commits
  do {
    if(!first) {
      // Get the previous commit
      commit_t* tmp_commit = reconstruct_commit(commit->parent);
      free(commit);
      commit = tmp_commit;
    }
    // Print out the commit hash
    printf("commit %s\n", commit->hash);

    // Print out the author
    printf("Author:\t%s\n", commit->author);

    // Calculate the date from the string
    struct tm tm;
    strptime(commit->timestamp, "%s %z", &tm);

    // Generate the time
    char time_string[50];
    strftime(time_string, 200, "%R %e %b %Y %z", &tm);

    // Print out the time
    printf("Date:\t%s\n\n", time_string);

    // Print out the commit message
    printf("    %s\n\n", commit->msg);
    first = false;
  } while(strncmp(commit->parent, "(null)", strlen("(null)")) != 0);
  free(commit);
}
