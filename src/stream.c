#include <stdlib.h>
#include <stdio.h>

#include "stream.h"

void open_memstream_safe(stream_t* stream) {
  stream->stream = open_memstream(&stream->buf, &stream->size);
  if(stream->stream == NULL) {
    perror("Unable to create file stream");
    exit(EXIT_FAILURE);
  }
}

void close_memstream(stream_t* stream) {
  fclose(stream->stream);
  free(stream->buf);
  stream->size = 0;
}

void rewind_memstream(stream_t* stream) {
  if(fflush(stream->stream) != 0) {
    perror("Stream could not be flushed");
    exit(EXIT_FAILURE);
  }
  rewind(stream->stream);
}

