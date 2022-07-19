#include <lzss/lzss.h>

#include "helper.h"

void lzss_decode_stream_open(lzss_decode_stream_t *stream,
                             lzss_stream_t *encoded) {
  stream->encoded = encoded;

  make_lcq(&stream->h_queue, stream->h_buffer, HISTORY_WINDOW_SIZE);

  stream->bitset = 0;
  stream->bit_index = 8;

  stream->pair_index = 0;
  stream->pair.start = 0;
  stream->pair.size = 0;
}

void lzss_decode_stream_close(lzss_decode_stream_t *stream) {
  stream->encoded = NULL;
}

int lzss_decode_stream_read(lzss_decode_stream_t *stream, void *buffer,
                            size_t size) {
  uint8_t tmp;
  size_t i, cnt;

  cnt = 0;
  while (cnt < size) {
    // if all pairwise data is read, put the cached data into the history window
    if (stream->pair_index != -1 && stream->pair_index == stream->pair.size) {
      for (i = 0; i < stream->pair.size; i++) {
        lcq_push_and_pop(&stream->h_queue, stream->pair_buffer[i]);
      }
    }

    // if unread pair data exists, read it
    if (stream->pair_index < stream->pair.size) {
      lcq_at(&stream->h_queue, stream->pair.start + stream->pair_index, &tmp);
      ((uint8_t *)buffer)[cnt] = tmp;
      // save to buffer, overwrite the history later
      stream->pair_buffer[stream->pair_index] = tmp;
      stream->pair_index++;
      cnt++;
      continue;
    }

    // if bitset is processing, read it
    if (stream->bit_index < 8) {
      if (!FLAG_AT(stream->bitset, stream->bit_index)) {
        // if bit is unset, read a literal byte
        stream->encoded->read(stream->encoded, &tmp, 1);
        ((uint8_t *)buffer)[cnt] = tmp;
        lcq_push_and_pop(&stream->h_queue, tmp);
        cnt++;
        stream->pair_index = -1;
      } else {
        stream->encoded->read(stream->encoded, &stream->pair, 2);
        stream->pair_index = 0;
      }

      stream->bit_index++;
      continue;
    }

    // no pending task, read a new bitset
    stream->encoded->read(stream->encoded, &stream->bitset, 1);
    stream->bit_index = 0;
  }

  return cnt;
}
