#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

// Define different areas within the shadow directory
#define SHADOW_DIR "/.teler/"
#define OBJECTS_DIR "/.teler/objects/"
#define REFS_DIR "/.teler/refs/"
#define HEADS_DIR "/.teler/refs/heads/"


char* get_objects_dir() {
  char* cwd = get_current_dir_name();

  cwd = (char*) realloc(cwd, strlen(cwd) + strlen(OBJECTS_DIR) + 1);
  strncat(cwd, OBJECTS_DIR, strlen(OBJECTS_DIR));
  
  return cwd;
}

char* get_refs_dir() {
  char* cwd = get_current_dir_name();

  cwd = (char*) realloc(cwd, strlen(cwd) + strlen(REFS_DIR) + 1);
  strncat(cwd, REFS_DIR, strlen(REFS_DIR));

  return cwd;
}

char* get_heads_dir() {
  char* cwd = get_current_dir_name();

  cwd = (char*) realloc(cwd, strlen(cwd) + strlen(HEADS_DIR) + 1);
  strncat(cwd, HEADS_DIR, strlen(HEADS_DIR));

  return cwd;
}

char* get_latest_commit(char* branch) {
  char* commit = get_heads_dir();

  size_t branch_len = strlen(branch);

  commit = (char*) realloc(commit, strlen(commit) + branch_len + 1);
  strncat(commit, branch, branch_len);
  return commit;
}

/* Purpose: Open a file pointed to by filePath with mode, mode, storing the result
   in fp and print an error message and exit if the file cannot be opened.*/
void open_file(FILE **fp, char *file_path, char *mode) {
  if ( (*fp = fopen(file_path, mode)) == NULL) {
    fprintf(stderr, "%s cannot be opened.\nExiting...\n", file_path);
    exit(EXIT_FAILURE);
  }
}

/* Purpose: Read one entire line from stream and store the result
   in lineptr. Replace the newline character at the end of the
   line with a null character instead.
   Preconditions:
   * stream is opened for reading. */
ssize_t readline(char **lineptr, FILE *stream)
{
  size_t len = 0;
  ssize_t chars = getline(lineptr, &len, stream);

  if((*lineptr)[chars-1] == '\n') {
    (*lineptr)[chars-1] = '\0';
    --chars;
  }

  return chars;
}

void open_object_file(FILE **fp, char* object, char* mode) {
  char* object_path = get_objects_dir();

  // Grow the length of the object path to fit in subdirectory
  size_t object_len = strlen(object);
  object_path = (char*) realloc(object_path, strlen(object_path) + object_len);

  // Concatenate on the first byte of the object
  strncat(object_path, object, 2);
  object_path[strlen(object_path)] = '/';

  // Construct the remaining path
  strncat(object_path, object + 2, 38);

  // Open the file
  open_file(fp, object_path, mode);

  // Free the constructed path name
  free(object_path);
}
