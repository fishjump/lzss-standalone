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

struct reference_pair {
  uint8_t start;
  uint8_t size;
};

struct lzss_decode_ctx {
  lzss_circular_queue_t h_queue; // history window queue

  uint8_t bitset;    // bitset for current flag byte
  uint8_t bit_index; // bit index for bitset

  size_t pair_index;     // index of the reference pair
  reference_pair_t pair; // reference pair

  uint8_t pair_buffer[HISTORY_WINDOW_SIZE];
};

void lzss_decode_ctx_init(lzss_decode_ctx_t *ctx, uint8_t *h_buffer,
                          size_t h_size);

int lzss_encode(lzss_stream_t *in, lzss_stream_t *out);
int lzss_decode(lzss_stream_t *in, lzss_stream_t *out, lzss_decode_ctx_t *ctx,
                size_t size);

#ifdef __cplusplus
}
#endif

#endif // __LZSS_STANDALONE_LZSS_H__