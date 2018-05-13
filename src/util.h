#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#include "data_structures/hash_map/hash_map.h"

char* get_objects_dir();
char* get_refs_dir();
char* get_heads_dir();

char* get_latest_commit(char* branch);

void open_file(FILE **fp, char *file_path, char *mode);

ssize_t readline(char **lineptr, FILE *stream);

void open_object_file(FILE **fp, char* object, char* mode);

type_t convert_to_type(char* type);
#endif
