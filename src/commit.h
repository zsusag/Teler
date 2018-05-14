#ifndef COMMIT_H
#define COMMIT_H

typedef struct commit {
  char* tree;
  char* parent;
  char* author;
  char* timestamp;
  char* msg;
  char* hash;
} commit_t;

void mk_commit(commit_t* c, char* tree, char* parent);

void destroy_commit(commit_t* c);

void write_commit(commit_t* c);

#endif
