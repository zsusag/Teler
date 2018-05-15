#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <openssl/sha.h>

#include "data_structures/hash_map/hash_map.h"

#define RWX_OWNER_PERM 0755

char* get_shadow_dir();
char* get_objects_dir();
char* get_refs_dir();
char* get_heads_dir();

void make_object_subdir(char* object);

char* get_latest_commit(char* branch);

void open_file(FILE **fp, char *file_path, char *mode);

ssize_t readline(char **lineptr, FILE *stream);

void open_object_file(FILE **fp, char* object, char* mode);

type_t convert_to_type(char* type);

void sha1_update_safe(SHA_CTX* c, const void* data, size_t len);

char* bin2hex(const unsigned char* bin, size_t len);

char* get_timestamp();

char* construct_filepath(char* prefix, char* suffix);
#endif
