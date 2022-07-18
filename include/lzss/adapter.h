#ifndef __LZSS_STANDALONE_ADAPTER_H__
#define __LZSS_STANDALONE_ADAPTER_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lzss_stream lzss_stream_t;

typedef int (*lzss_read_func_t)(lzss_stream_t *s, void *buffer, size_t size);
typedef int (*lzss_write_funct_t)(lzss_stream_t *s, void *buffer, size_t size);

struct lzss_stream {
  void *opaque;
  lzss_read_func_t read;
  lzss_write_funct_t write;
};

#ifdef __cplusplus
}
#endif

#endif // __LZSS_STANDALONE_ADAPTER_H__