#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>

#include "data_structures/hash_map/hash_map.h"
#include "util.h"

// Define different areas within the shadow directory
#define SHADOW_DIR "/.teler/"
#define OBJECTS_DIR "/.teler/objects/"
#define REFS_DIR "/.teler/refs/"
#define HEADS_DIR "/.teler/refs/heads/"

char* get_shadow_dir() {
  char* cwd = get_current_dir_name();

  cwd = (char*) realloc(cwd, strlen(cwd) + strlen(SHADOW_DIR) + 1);
  strncat(cwd, SHADOW_DIR, strlen(SHADOW_DIR));
  
  return cwd;

}

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

void make_object_subdir(char* object) {
  // Retrieve the name of the the objects directory
  char* dir = get_objects_dir();

  // Concatenate the first byte of the object
  size_t dir_len = strlen(dir);
  // 3 = 2 for first byte + 1 for null byte
  dir = (char*) realloc(dir, dir_len + 3);
  strncat(dir, object, 2);

  dir[dir_len + 2] = '\0';

  // Check to see if the directory has been created already, if not, create it
  struct stat st_dir = {0};
  if(stat(dir, &st_dir) == -1) {
    if(mkdir(dir, RWX_OWNER_PERM) != 0) {
      perror("Unable to create object subdirectory");
      exit(EXIT_FAILURE);
    }
  }

  free(dir);
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
  // Check to make sure the object subdirectory is there
  make_object_subdir(object);

  char* object_path = get_objects_dir();

  // Grow the length of the object path to fit in subdirectory
  size_t object_len = strlen(object);
  object_path = (char*) realloc(object_path, strlen(object_path) + object_len + 2);

  // Concatenate on the first byte of the object
  strncat(object_path, object, 2);
  strncat(object_path, "/", 1);
  //  object_path[strlen(object_path)] = '/';

  // Construct the remaining path
  strncat(object_path, object + 2, 38);

  // Put the null byte at the end
  object_path[strlen(object_path) + object_len] = '\0';

  // Open the file
  open_file(fp, object_path, mode);

  // Free the constructed path name
  free(object_path);
}

type_t convert_to_type(char* type) {
  if(strncmp(type, "blob", 4) == 0) {
    return blob;
  } else {
    return tree;
  }
}

void sha1_update_safe(SHA_CTX* c, const void* data, size_t len) {
  if(SHA1_Update(c, data, len) == 0) {
    fprintf(stderr, "Unable to update SHA1 hash\n");
    exit(EXIT_FAILURE);
  }
}

char* bin2hex(const unsigned char* bin, size_t len) {
  // Check to see if bin is null or has a length of 0
  if(bin == NULL || len == 0) {
    return NULL;
  }

  // Allocate enough memory for the hexidecimal representation of bin
  char* hex = (char*) malloc(2*len + 1);
  for(size_t i = 0; i < len; i++) {
    hex[i*2] = "0123456789abcdcef"[bin[i] >> 4];
    hex[i*2+1] = "0123456789abcdef"[bin[i] & 0x0F];
  }
  hex[len*2] = '\0';

  return hex;
}

char* get_timestamp() {
  // Get the current time
  time_t t = time(NULL);
  if(t == ((time_t) -1)) {
    perror("Unable to get current time");
    exit(EXIT_FAILURE);
  }

  // Declare a struct to hold the local time and populate it
  struct tm* lt = localtime(&t);
  if(lt == NULL) {
    perror("Unable to convert to localtime");
    exit(EXIT_FAILURE);
  }

  char* ret = (char*) malloc(200);
  strftime(ret, 200, "%s %z", lt);
  return ret;
}
