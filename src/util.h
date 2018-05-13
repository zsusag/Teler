#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

char* get_objects_dir();
char* get_refs_dir();
char* get_heads_dir();

char* get_latest_commit(char* branch);

void open_file(FILE **fp, char *file_path, char *mode);

ssize_t readline(char **lineptr, FILE *stream);

void open_object_file(FILE **fp, char* object, char* mode);
#endif
