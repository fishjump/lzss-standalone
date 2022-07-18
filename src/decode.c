#include <lzss/lzss.h>

#include "circular_queue.h"
#include "helper.h"

int lzss_decode(lzss_stream_t *in, lzss_stream_t *out, size_t size) {
  uint8_t h_buf[HISTORY_WINDOW_SIZE]; // history window
  uint8_t f_buf[FUTURE_WINDOW_SIZE];  // future window

  lzss_circular_queue_t h_queue; // history window queue
  lzss_circular_queue_t f_queue; // future window queue

  reference_pair_t pair;
  lzss_block_stream_t lbs;

  uint8_t c; // current/old character
  uint8_t buffer[HISTORY_WINDOW_SIZE];
  uint8_t bitset;
  size_t i, j;

  make_lcq(&h_queue, h_buf, HISTORY_WINDOW_SIZE);
  make_lcq(&f_queue, f_buf, FUTURE_WINDOW_SIZE);

  while (size > 0 && in->read(in, &bitset, 1) == 1) {
    for (i = 0; size > 0 && i < 8; i++) {
      if (!FLAG_AT(bitset, i)) {
        // if bit is unset, read a literal byte
        ASSERT_EQ(in->read(in, &c, 1), 1);
        ASSERT_EQ(out->write(out, &c, 1), 1);
        ASSERT_EQ(lcq_push_and_pop(&h_queue, c), 0);
        size--;
        continue;
      }

      // if bit is set, read a reference pair
      ASSERT_EQ(in->read(in, &pair, 2), 2);
      for (j = 0; size > 0 && j < pair.size; j++) {
        ASSERT_EQ(lcq_at(&h_queue, pair.start + j, &c), 0);
        ASSERT_EQ(out->write(out, &c, 1), 1);
        buffer[j] = c; // save to buffer, overwrite the history later
        size--;
      }

      for (j = 0; size > 0 && j < pair.size; j++) {
        ASSERT_EQ(lcq_push_and_pop(&h_queue, buffer[j]), 0);
      }
    }
  }

  return 0;
}