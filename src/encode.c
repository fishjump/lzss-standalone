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

void lzss_encode_stream_open(lzss_encode_stream_t *stream, lzss_stream_t *out) {
  stream->out = out;

  make_lcq(&stream->h_queue, stream->h_buf, HISTORY_WINDOW_SIZE);
  make_lcq(&stream->f_queue, stream->f_buf, FUTURE_WINDOW_SIZE);
  make_lbs(&stream->lbs);
}

int lzss_encode_stream_write(lzss_encode_stream_t *stream, void *buffer,
                             size_t size) {
  size_t i, j;
  uint8_t c, o;
  reference_pair_t pair;

  for (i = 0; i < size; i++) {
    // stream->origin->read(stream->origin, &c, 1);
    c = ((uint8_t *)buffer)[i];

    if (!lcq_full(&stream->f_queue)) {
      lcq_push(&stream->f_queue, c);
      continue;
    }

    pair = lcp(&stream->f_queue, &stream->h_queue);

    if (pair.size <= 2) {
      // put the front to the history and write it to the output. the reference
      // pair takes 2 bytes, we won't be benefited if the common prefix is not
      // greater than 3. in this case, write the literal character.
      lcq_front(&stream->f_queue, &o);
      lcq_pop(&stream->f_queue);
      lcq_push_and_pop(&stream->h_queue, o);

      lbs_write_byte(&stream->lbs, o);
    } else {
      // save the reference pair to the output, and put them into the history
      for (j = 0; j < pair.size; j++) {
        ASSERT_EQ(lcq_front(&stream->f_queue, &o), 0);
        ASSERT_EQ(lcq_pop(&stream->f_queue), 0);
        ASSERT_EQ(lcq_push_and_pop(&stream->h_queue, o), 0);
      }

      lbs_write_pair(&stream->lbs, pair);
    }

    if (lbs_eof(&stream->lbs)) {
      stream->out->write(stream->out, &stream->lbs.block,
                         lbs_size(&stream->lbs));
      make_lbs(&stream->lbs); // reset lbs
    }

    // add the current character to the future window
    lcq_push(&stream->f_queue, c);
  }

  return i;
}

void lzss_encode_stream_close(lzss_encode_stream_t *stream) {
  uint8_t c, o;
  size_t i;
  reference_pair_t pair;
  while (!lcq_empty(&stream->f_queue)) {
    pair = lcp(&stream->f_queue, &stream->h_queue);
    if (pair.size <= 2) {
      lcq_front(&stream->f_queue, &o);
      lcq_pop(&stream->f_queue);
      lcq_push_and_pop(&stream->h_queue, o);
      lbs_write_byte(&stream->lbs, o);
    } else {
      for (i = 0; i < pair.size; i++) {
        lcq_front(&stream->f_queue, &o);
        lcq_pop(&stream->f_queue);
        lcq_push_and_pop(&stream->h_queue, o);
      }
      lbs_write_pair(&stream->lbs, pair);
    }

    if (lbs_eof(&stream->lbs)) {
      stream->out->write(stream->out, &stream->lbs.block,
                         lbs_size(&stream->lbs));
      make_lbs(&stream->lbs); // reset lbs
    }
  }

  stream->out->write(stream->out, &stream->lbs.block, lbs_size(&stream->lbs));
}
