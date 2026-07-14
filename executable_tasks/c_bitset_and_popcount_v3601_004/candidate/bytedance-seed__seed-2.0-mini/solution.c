#include "interface.h"
#include <stdint.h>

uint64_t run(const uint64_t* restrict left, const uint64_t* restrict right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    total = 0;
    size_t i = 0;
    // Unroll loop to reduce overhead and improve ILP
    for (; i + 3 < word_count; i += 4) {
      total += __builtin_popcountll(left[i] & right[i]);
      total += __builtin_popcountll(left[i+1] & right[i+1]);
      total += __builtin_popcountll(left[i+2] & right[i+2]);
      total += __builtin_popcountll(left[i+3] & right[i+3]);
    }
    // Handle remaining elements
    for (; i < word_count; ++i) {
      total += __builtin_popcountll(left[i] & right[i]);
    }
  }
  return total;
}