#ifndef __LZSS_STANDALONE_ADAPTERS_ARRAY_LIKE_H__
#define __LZSS_STANDALONE_ADAPTERS_ARRAY_LIKE_H__

#include <stdint.h>

#include <stdio.h>

#include "../adapter.h"

#define __lzss_weak __attribute__((weak))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct array_like array_like_t;

struct array_like {
  uint8_t *data;
  size_t cursor;
  size_t size;
};

void make_array_like(array_like_t *array, uint8_t *data, size_t size);
void open_array_like_stream(lzss_stream_t *s, array_like_t *array);
void close_array_like_stream(lzss_stream_t *s);
size_t array_like_read(lzss_stream_t *s, void *buffer, size_t size);
size_t array_like_write(lzss_stream_t *s, void *buffer, size_t size);

__lzss_weak void make_array_like(array_like_t *array, uint8_t *data,
                                 size_t size) {
  array->data = data;
  array->size = size;
  array->cursor = 0;
}

__lzss_weak void open_array_like_stream(lzss_stream_t *s, array_like_t *array) {
  s->opaque = array;
  s->read = array_like_read;
  s->write = array_like_write;
}

__lzss_weak void close_array_like_stream(lzss_stream_t *s) {
  array_like_t *array;

  array = (array_like_t *)s->opaque;
  array->cursor = 0;
}

__lzss_weak size_t array_like_read(lzss_stream_t *s, void *buffer, size_t size) {
  array_like_t *array;
  size_t i;

  array = (array_like_t *)s->opaque;

  if (!(array->cursor < array->size)) {
    return 0;
  }

  for (i = 0; i < size; i++) {
    if (!(array->cursor < array->size)) {
      break;
    }
    ((uint8_t *)buffer)[i] = array->data[array->cursor];
    array->cursor++;
  }

  return i;
}

__lzss_weak size_t array_like_write(lzss_stream_t *s, void *buffer, size_t size) {
  array_like_t *array;
  size_t i;

  array = (array_like_t *)s->opaque;

  if (!(array->cursor < array->size)) {
    return 0;
  }

  for (i = 0; i < size; i++) {
    if (!(array->cursor < array->size)) {
      break;
    }
    array->data[array->cursor] = ((uint8_t *)buffer)[i];
    array->cursor++;
  }

  return i;
}

#ifdef __cplusplus
}
#endif

#endif // __LZSS_STANDALONE_ADAPTERS_ARRAY_LIKE_H__