#ifndef COMPRESSION_H
#define COMPRESSION_H
#include <stdio.h>

#define CHUNK 16384

int def(FILE* source, FILE* dest, int level);
int inf(FILE* source, FILE* dest);
void zerr(int ret);
#endif
