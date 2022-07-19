#ifndef __LZSS_STANDALONE_HELPER_H__
#define __LZSS_STANDALONE_HELPER_H__

#include <lzss/lzss.h>
#include <lzss/config.h>

#define ASSERT_EQ(a, b)                                                        \
  do {                                                                         \
    if ((a) != (b)) {                                                          \
      return -1;                                                               \
    }                                                                          \
  } while (0)

#define FLAG_AT(bitset, index) (((bitset) >> (index)) & 0b00000001)
#define FLAG_SET(bitset, index) ((bitset) |= (0b00000001 << (index)))
#define FLAG_UNSET(bitset, index) ((bitset) &= ~(0b00000001 << (index)))

typedef struct lzss_block lzss_block_t;

struct lzss_block {
  uint8_t flag;
  uint8_t data[16];
};

typedef struct lzss_block_stream lzss_block_stream_t;

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

#endif // __LZSS_STANDALONE_HELPER_H__
