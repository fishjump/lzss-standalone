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
typedef struct lzss_block lzss_block_t;
typedef struct lzss_block_stream lzss_block_stream_t;

struct reference_pair {
  uint8_t start;
  uint8_t size;
};

struct lzss_block {
  uint8_t flag;
  uint8_t data[16];
};

struct lzss_block_stream {
  lzss_block_t block;
  size_t cursor;
  size_t item_cnt;
};

void make_lbs(lzss_block_stream_t *stream);
int lbs_write_byte(lzss_block_stream_t *stream, uint8_t data);
int lbs_write_pair(lzss_block_stream_t *stream, reference_pair_t pair);
int lbs_size(lzss_block_stream_t *stream);
int lbs_eof(lzss_block_stream_t *stream);

struct lzss_encode_stream {
  lzss_stream_t *out;

  uint8_t h_buf[HISTORY_WINDOW_SIZE]; // history window
  uint8_t f_buf[FUTURE_WINDOW_SIZE];  // future window

  lzss_circular_queue_t h_queue; // history window queue
  lzss_circular_queue_t f_queue; // future window queue

  // reference_pair_t pair;
  lzss_block_stream_t lbs;
};

int lzss_encode_stream_write(lzss_encode_stream_t *stream, void *buffer,
                             size_t size);
void lzss_encode_stream_open(lzss_encode_stream_t *stream,
                             lzss_stream_t *encoded);
void lzss_encode_stream_close(lzss_encode_stream_t *stream);

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