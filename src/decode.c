#include <lzss/lzss.h>

#include "helper.h"

#include <stdio.h>

void lzss_decode_ctx_init(lzss_decode_ctx_t *ctx, uint8_t *h_buffer,
                          size_t h_size) {
  make_lcq(&ctx->h_queue, h_buffer, h_size);

  ctx->bitset = 0;
  ctx->bit_index = 8;
  ctx->pair_index = 0;
  ctx->pair.start = 0;
  ctx->pair.size = 0;
}

int lzss_decode(lzss_stream_t *in, lzss_stream_t *out, lzss_decode_ctx_t *ctx,
                size_t size) {
  uint8_t tmp;
  size_t i;

  while (size > 0) {
    // if all pairwise data is read, put the cached data into the history window
    if (ctx->pair_index != -1 && ctx->pair_index == ctx->pair.size) {
      for (i = 0; i < ctx->pair.size; i++) {
        lcq_push_and_pop(&ctx->h_queue, ctx->pair_buffer[i]);
      }
    }

    // if unread pair data exists, read it
    if (ctx->pair_index < ctx->pair.size) {
      lcq_at(&ctx->h_queue, ctx->pair.start + ctx->pair_index, &tmp);
      out->write(out, &tmp, 1);
      // save to buffer, overwrite the history later
      ctx->pair_buffer[ctx->pair_index] = tmp;
      ctx->pair_index++;
      size--;
      continue;
    }

    // if bitset is processing, read it
    if (ctx->bit_index < 8) {
      if (!FLAG_AT(ctx->bitset, ctx->bit_index)) {
        // if bit is unset, read a literal byte
        if (in->read(in, &tmp, 1) != 1) {
          printf("read failed\n");
        }
        out->write(out, &tmp, 1);
        printf("literal: 0x%02x \n", tmp);
        lcq_push_and_pop(&ctx->h_queue, tmp);
        size--;
        ctx->pair_index = -1;
      } else {
        in->read(in, &ctx->pair, 2);
        ctx->pair_index = 0;
      }

      ctx->bit_index++;
      continue;
    }

    // no pending task, read a new bitset
    in->read(in, &ctx->bitset, 1);
    ctx->bit_index = 0;
  }

  return 0;
}