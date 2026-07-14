#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint64_t popcount_and_sum(const uint64_t* left, const uint64_t* right, size_t n) {
  uint64_t total = 0;
  size_t i = 0;

  // Unroll for throughput
  for (; i + 8 <= n; i += 8) {
    uint64_t a0 = left[i+0] & right[i+0];
    uint64_t a1 = left[i+1] & right[i+1];
    uint64_t a2 = left[i+2] & right[i+2];
    uint64_t a3 = left[i+3] & right[i+3];
    uint64_t a4 = left[i+4] & right[i+4];
    uint64_t a5 = left[i+5] & right[i+5];
    uint64_t a6 = left[i+6] & right[i+6];
    uint64_t a7 = left[i+7] & right[i+7];
    total += (uint64_t)__builtin_popcountll(a0);
    total += (uint64_t)__builtin_popcountll(a1);
    total += (uint64_t)__builtin_popcountll(a2);
    total += (uint64_t)__builtin_popcountll(a3);
    total += (uint64_t)__builtin_popcountll(a4);
    total += (uint64_t)__builtin_popcountll(a5);
    total += (uint64_t)__builtin_popcountll(a6);
    total += (uint64_t)__builtin_popcountll(a7);
  }
  for (; i < n; ++i) {
    total += (uint64_t)__builtin_popcountll(left[i] & right[i]);
  }
  return total;
}

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  if (iters <= 0) return 0;
  return popcount_and_sum(left, right, word_count);
}
