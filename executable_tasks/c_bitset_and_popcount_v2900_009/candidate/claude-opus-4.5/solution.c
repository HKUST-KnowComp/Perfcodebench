#include "interface.h"

#include <stdint.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    size_t i = 0;
    size_t limit = word_count & ~(size_t)3;
    for (; i < limit; i += 4) {
      count += (uint64_t)__builtin_popcountll(left[i] & right[i]);
      count += (uint64_t)__builtin_popcountll(left[i + 1] & right[i + 1]);
      count += (uint64_t)__builtin_popcountll(left[i + 2] & right[i + 2]);
      count += (uint64_t)__builtin_popcountll(left[i + 3] & right[i + 3]);
    }
    for (; i < word_count; ++i) {
      count += (uint64_t)__builtin_popcountll(left[i] & right[i]);
    }
    total = count;
  }
  return total;
}