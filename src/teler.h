#ifndef TELER_H
#define TELER_H

#define BLOCK_SIZE 4096
#define SHADOW_DIR "/.teler/"


// Initialize an empty teler repository
void teler_init();

// Push commits to the shadow directory and to remotes
void push();

#endif
