#ifndef STREAM_H
#define STREAM_H

#include <stdio.h>

typedef struct stream {
  char* buf;
  FILE* stream;
  size_t size;
} stream_t;

void open_memstream_safe(stream_t* stream);

void close_memstream(stream_t* stream);

#endif
