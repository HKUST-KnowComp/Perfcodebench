#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    size_t i = 0;
    /* Unroll by 4 for better ILP */
    size_t limit = word_count & ~(size_t)3;
    for (; i < limit; i += 4) {
      uint64_t w0 = left[i]     & right[i];
      uint64_t w1 = left[i + 1] & right[i + 1];
      uint64_t w2 = left[i + 2] & right[i + 2];
      uint64_t w3 = left[i + 3] & right[i + 3];
      count += (uint64_t)__builtin_popcountll(w0)
             + (uint64_t)__builtin_popcountll(w1)
             + (uint64_t)__builtin_popcountll(w2)
             + (uint64_t)__builtin_popcountll(w3);
    }
    for (; i < word_count; ++i) {
      count += (uint64_t)__builtin_popcountll(left[i] & right[i]);
    }
    total = count;
  }
  return total;
}
