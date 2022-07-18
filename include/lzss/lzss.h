#ifndef __LZSS_STANDALONE_LZSS_H__
#define __LZSS_STANDALONE_LZSS_H__

#include <stdint.h>

#include "adapter.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

int lzss_encode(lzss_stream_t *in, lzss_stream_t *out);
int lzss_decode(lzss_stream_t *in, lzss_stream_t *out, size_t size);

#ifdef __cplusplus
}
#endif

#endif // __LZSS_STANDALONE_LZSS_H__