#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "teler.h"
#include "util.h"
/* Things that init should do:
   1) Create an empty .teler directory in the current working directory
   2) Put an initial commit with nothing in it
*/

void teler_init() {
  // Retrieve the name of the current working directory
  char *cwd = get_current_dir_name();

  // Concatenate "/.teler/" onto the cwd
  cwd = (char*) realloc(cwd, strlen(cwd) + strlen(SHADOW_DIR) + 1);
  strncat(cwd, SHADOW_DIR, strlen(SHADOW_DIR));

  /* Check to see if the directory has already been created. If so, do nothing.
     If not, then create an empty repository */
  struct stat st_shadow_dir = {0};
  if(stat(cwd, &st_shadow_dir) == -1) {
    if(mkdir(cwd, RWX_OWNER_PERM) != 0) {
      perror("Unable to create .teler directory");
      exit(EXIT_FAILURE);
    }

    // Allocate memory for objects and refs directories
    char* object_dir = (char*) malloc(strlen(cwd) + strlen("objects/") + 1);
    char* refs_dir   = (char*) malloc(strlen(cwd) + strlen("refs/") + 1);

    // Copy the cwd and append corresponding directory suffix
    strncpy(object_dir, cwd, strlen(cwd));
    strncat(object_dir, "objects/", strlen("objects/"));

    strncpy(refs_dir, cwd, strlen(cwd));
    strncat(refs_dir, "refs/", strlen("refs/"));

    // Allocate memory for head dir within refs directory
    char* refs_head_dir = (char*) malloc(strlen(refs_dir) + strlen("heads/") + 1);

    // Copy the refs dir and append head dir
    strncpy(refs_head_dir, refs_dir, strlen(refs_dir));
    strncat(refs_head_dir, "heads/", strlen("heads/"));

    // Create the objects, refs, and refs/heads directory
    if(mkdir(object_dir, RWX_OWNER_PERM) != 0) {
      perror("Unable to create objects directory");
      exit(EXIT_FAILURE);
    }
    if(mkdir(refs_dir, RWX_OWNER_PERM) != 0) {
      perror("Unable to create refs directory");
      exit(EXIT_FAILURE);
    }
    if(mkdir(refs_head_dir, RWX_OWNER_PERM) != 0) {
      perror("Unable to create refs/heads directory");
      exit(EXIT_FAILURE);
    }

    // Prompt user to enter name and email
    char* name;
    char* email;
    printf("Please enter your name followed by <ENTER>: ");
    readline(&name, stdin);
    printf("Please enter your email followed by <ENTER>: ");
    readline(&email, stdin);

    char* config_path = (char*) malloc(strlen(cwd) + strlen("config") + 1);
    strncpy(config_path, cwd, strlen(cwd));
    strncat(config_path, "config", strlen("config"));

    FILE* fp;
    open_file(&fp, config_path, "w");

    fprintf(fp, "%s %s", name, email);
    fclose(fp);

    printf("Initialized empty teler repository in %s\n", cwd);
    free(config_path);
    free(name);
    free(email);
    free(object_dir);
    free(refs_dir);
    free(refs_head_dir);
  }
  free(cwd);
}
