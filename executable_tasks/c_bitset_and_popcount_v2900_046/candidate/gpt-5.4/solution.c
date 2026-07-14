#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;

  if (iters <= 0 || word_count == 0) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum = 0;
    size_t i = 0;

#if defined(__GNUC__) || defined(__clang__)
    for (; i + 4 <= word_count; i += 4) {
      sum += (uint64_t)__builtin_popcountll(left[i] & right[i]);
      sum += (uint64_t)__builtin_popcountll(left[i + 1] & right[i + 1]);
      sum += (uint64_t)__builtin_popcountll(left[i + 2] & right[i + 2]);
      sum += (uint64_t)__builtin_popcountll(left[i + 3] & right[i + 3]);
    }
    for (; i < word_count; ++i) {
      sum += (uint64_t)__builtin_popcountll(left[i] & right[i]);
    }
#else
    for (; i < word_count; ++i) {
      uint64_t x = left[i] & right[i];
      x = x - ((x >> 1) & 0x5555555555555555ULL);
      x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
      x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
      sum += (x * 0x0101010101010101ULL) >> 56;
    }
#endif

    total = sum;
  }

  return total;
}
