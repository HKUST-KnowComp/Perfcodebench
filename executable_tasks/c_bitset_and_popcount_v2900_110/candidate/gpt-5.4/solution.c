#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

#if defined(__GNUC__) || defined(__clang__)
#define HAVE_BUILTIN_POPCOUNTLL 1
#else
#define HAVE_BUILTIN_POPCOUNTLL 0
#endif

static inline uint64_t popcount64(uint64_t x) {
#if HAVE_BUILTIN_POPCOUNTLL
  return (uint64_t)__builtin_popcountll((unsigned long long)x);
#else
  x = x - ((x >> 1) & 0x5555555555555555ULL);
  x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
  x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
  return (x * 0x0101010101010101ULL) >> 56;
#endif
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;

  if (iters <= 0 || word_count == 0) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint64_t* l = left;
    const uint64_t* r = right;
    size_t i = 0;
    uint64_t sum = 0;

    for (; i + 4 <= word_count; i += 4) {
      sum += popcount64(l[0] & r[0]);
      sum += popcount64(l[1] & r[1]);
      sum += popcount64(l[2] & r[2]);
      sum += popcount64(l[3] & r[3]);
      l += 4;
      r += 4;
    }

    for (; i < word_count; ++i) {
      sum += popcount64(*l++ & *r++);
    }

    total = sum;
  }

  return total;
}
