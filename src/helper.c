#include "helper.h"

void make_lbs(lzss_block_stream_t *stream) {
  stream->cursor = 0;
  stream->item_cnt = 0;
}

int lbs_write_byte(lzss_block_stream_t *stream, uint8_t data) {
  if (lbs_eof(stream)) {
    return -1;
  }

  // unset for literal byte data
  FLAG_UNSET(stream->block.flag, stream->item_cnt);

  stream->block.data[stream->cursor] = data;
  stream->cursor++;

  stream->item_cnt++;

  return 0;
}

int lbs_write_pair(lzss_block_stream_t *stream, reference_pair_t pair) {
  if (lbs_eof(stream)) {
    return -1;
  }

  // set for reference pair
  FLAG_SET(stream->block.flag, stream->item_cnt);

  stream->block.data[stream->cursor] = pair.start;
  stream->cursor++;
  stream->block.data[stream->cursor] = pair.size;
  stream->cursor++;

  stream->item_cnt++;

  return 0;
}

int lbs_size(lzss_block_stream_t *stream) { return stream->cursor + 1; }

int lbs_eof(lzss_block_stream_t *stream) { return stream->item_cnt == 8; }