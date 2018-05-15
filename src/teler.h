#ifndef TELER_H
#define TELER_H

#define BLOCK_SIZE 4096
#define SHADOW_DIR "/.teler/"


// Initialize an empty teler repository
void teler_init();

// Push commits to the shadow directory and to remotes
void push();

// Pull the latest commit into the working directory
void pull(char* commit);

// Print out the entire history of the repository
void history();
#endif
