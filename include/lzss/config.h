#ifndef __LZSS_STANDALONE_CONFIG_H__
#define __LZSS_STANDALONE_CONFIG_H__

#include <stddef.h>
#include <stdint.h>

#define HISTORY_WINDOW_SIZE (8)
#define FUTURE_WINDOW_SIZE (8)

#if HISTORY_WINDOW_SIZE > UINT8_MAX || FUTURE_WINDOW_SIZE > UINT8_MAX
#error HISTORY_WINDOW_SIZE and FUTURE_WINDOW_SIZE are expected to be less than UINT8_MAX
#endif

#endif // __LZSS_STANDALONE_CONFIG_H__