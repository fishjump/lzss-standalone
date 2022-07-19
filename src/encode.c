#include <lzss/lzss.h>

#include "helper.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

/**
 * @brief find the longest match for two circular queues, given two start
 * indices.
 *
 * @param a sequence of bytes a
 * @param a_start start index of a
 * @param b sequence of bytes b
 * @param b_start start index of b
 * @param max_len minimum length between a and b
 * @return int the length of matched bytes
 */
static int match_len(const lzss_circular_queue_t *a, size_t a_start,
                     const lzss_circular_queue_t *b, size_t b_start,
                     size_t max_len) {
  size_t i;
  uint8_t a_value, b_value;

  for (i = 0; i < max_len; i++) {
    if (lcq_at(a, a_start + i, &a_value) != 0 ||
        lcq_at(b, b_start + i, &b_value) != 0) {
      return i;
    }

    if (a_value != b_value) {
      return i;
    }
  }
  return max_len;
}

/**
 * @brief longest common prefix. start from a[head], to find out the longest
 * common substring in b.
 *
 * @param a sequence of bytes a
 * @param b sequence of bytes b
 * @return int the length of longest common substring
 */
static reference_pair_t lcp(const lzss_circular_queue_t *a,
                            const lzss_circular_queue_t *b) {
  size_t i;
  size_t a_size, b_size;
  size_t max_len;
  size_t match_cnt;

  reference_pair_t ret;

  a_size = lcq_size(a);
  b_size = lcq_size(b);

  max_len = MIN(a_size, b_size);
  ret.size = 0;
  for (i = 0; i < b_size; i++) {
    match_cnt = match_len(a, 0, b, i, max_len);
    if (match_cnt > ret.size) {
      ret.size = match_cnt;
      ret.start = i;
    }
  }

  return ret;
}

int lzss_encode(lzss_stream_t *in, lzss_stream_t *out) {
  uint8_t h_buf[HISTORY_WINDOW_SIZE]; // history window
  uint8_t f_buf[FUTURE_WINDOW_SIZE];  // future window

  lzss_circular_queue_t h_queue; // history window queue
  lzss_circular_queue_t f_queue; // future window queue

  reference_pair_t pair;
  lzss_block_stream_t lbs;

  uint8_t c, o; // current/old character

  size_t i;

  make_lcq(&h_queue, h_buf, HISTORY_WINDOW_SIZE);
  make_lcq(&f_queue, f_buf, FUTURE_WINDOW_SIZE);
  make_lbs(&lbs);

  // read until EOF
  while (in->read(in, &c, 1) == 1) {
    // if the future window is not full, push the current character into it
    // until it is full.
    if (!lcq_full(&f_queue)) {
      ASSERT_EQ(lcq_push(&f_queue, c), 0);
      continue;
    }

    // find the longest common prefix in the history for the future window
    pair = lcp(&f_queue, &h_queue);

    if (pair.size <= 2) {
      // put the front to the history and write it to the output. the reference
      // pair takes 2 bytes, we won't be benefited if the common prefix is not
      // greater than 3. in this case, write the literal character.
      ASSERT_EQ(lcq_front(&f_queue, &o), 0);
      ASSERT_EQ(lcq_pop(&f_queue), 0);
      ASSERT_EQ(lcq_push_and_pop(&h_queue, o), 0);

      ASSERT_EQ(lbs_write_byte(&lbs, o), 0);
    } else {
      // save the reference pair to the output, and put them into the history
      for (i = 0; i < pair.size; i++) {
        ASSERT_EQ(lcq_front(&f_queue, &o), 0);
        ASSERT_EQ(lcq_pop(&f_queue), 0);
        ASSERT_EQ(lcq_push_and_pop(&h_queue, o), 0);
      }

      ASSERT_EQ(lbs_write_pair(&lbs, pair), 0);
    }

    if (lbs_eof(&lbs)) {
      out->write(out, &lbs.block, lbs_size(&lbs));
      make_lbs(&lbs); // reset lbs
    }

    // add the current character to the future window
    ASSERT_EQ(lcq_push(&f_queue, c), 0);
  }

  // cleanup the rest of data in the future window
  while (!lcq_empty(&f_queue)) {

    pair = lcp(&f_queue, &h_queue);

    if (pair.size <= 2) {
      ASSERT_EQ(lcq_front(&f_queue, &o), 0);
      ASSERT_EQ(lcq_pop(&f_queue), 0);
      ASSERT_EQ(lcq_push_and_pop(&h_queue, o), 0);

      ASSERT_EQ(lbs_write_byte(&lbs, o), 0);
    } else {
      for (i = 0; i < pair.size; i++) {
        ASSERT_EQ(lcq_front(&f_queue, &o), 0);
        ASSERT_EQ(lcq_pop(&f_queue), 0);
        ASSERT_EQ(lcq_push_and_pop(&h_queue, o), 0);
      }
      ASSERT_EQ(lbs_write_pair(&lbs, pair), 0);
    }

    if (lbs_eof(&lbs)) {
      out->write(out, &lbs.block, lbs_size(&lbs));
      make_lbs(&lbs); // reset lbs
    }
  }

  out->write(out, &lbs.block, lbs_size(&lbs));

  return 0;
}