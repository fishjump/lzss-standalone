#ifndef __LZSS_STANDALONE_LZSS_H__
#define __LZSS_STANDALONE_LZSS_H__

#include <stdint.h>

#include "adapter.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lzss_decode_ctx lzss_decode_ctx_t;
typedef struct reference_pair reference_pair_t;
typedef struct lzss_encode_stream lzss_encode_stream_t;
typedef struct lzss_decode_stream lzss_decode_stream_t;

struct reference_pair {
  uint8_t start;
  uint8_t size;
};

// TODO: implement this struct
// struct lzss_encode_stream {
//   lzss_stream_t *origin;

//   uint8_t h_buffer[HISTORY_WINDOW_SIZE];
//   lzss_circular_queue_t h_queue; // history window queue

//   uint8_t bitset;    // bitset for current flag byte
//   uint8_t bit_index; // bit index for bitset

//   size_t pair_index;     // index of the reference pair
//   reference_pair_t pair; // reference pair

//   uint8_t pair_buffer[HISTORY_WINDOW_SIZE];
// };

// int lzss_encode_stream_read(lzss_encode_stream_t *stream, void *buffer,
//                             size_t size);
// void lzss_encode_stream_open(lzss_encode_stream_t *stream,
//                              lzss_stream_t *encoded);
// void lzss_encode_stream_close(lzss_encode_stream_t *stream);

int lzss_encode(lzss_stream_t *in, lzss_stream_t *out);

struct lzss_decode_stream {
  lzss_stream_t *encoded;

  uint8_t h_buffer[HISTORY_WINDOW_SIZE];
  lzss_circular_queue_t h_queue; // history window queue

  uint8_t bitset;    // bitset for current flag byte
  uint8_t bit_index; // bit index for bitset

  size_t pair_index;     // index of the reference pair
  reference_pair_t pair; // reference pair

  uint8_t pair_buffer[HISTORY_WINDOW_SIZE];
};

int lzss_decode_stream_read(lzss_decode_stream_t *stream, void *buffer,
                            size_t size);
void lzss_decode_stream_open(lzss_decode_stream_t *stream,
                             lzss_stream_t *encoded);
void lzss_decode_stream_close(lzss_decode_stream_t *stream);

#ifdef __cplusplus
}
#endif

#endif // __LZSS_STANDALONE_LZSS_H__