#ifndef __LZSS_STANDALONE_HELPER_H__
#define __LZSS_STANDALONE_HELPER_H__

#include <lzss/config.h>
#include <lzss/lzss.h>

#define ASSERT_EQ(a, b)                                                        \
  do {                                                                         \
    if ((a) != (b)) {                                                          \
      return -1;                                                               \
    }                                                                          \
  } while (0)

#define FLAG_AT(bitset, index) (((bitset) >> (index)) & 0b00000001)
#define FLAG_SET(bitset, index) ((bitset) |= (0b00000001 << (index)))
#define FLAG_UNSET(bitset, index) ((bitset) &= ~(0b00000001 << (index)))

#endif // __LZSS_STANDALONE_HELPER_H__
