#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current = 0;
    size_t i = 0;
    for (; i + 7 < word_count; i += 8) {
      current += __builtin_popcountll(left[i] & right[i]);
      current += __builtin_popcountll(left[i+1] & right[i+1]);
      current += __builtin_popcountll(left[i+2] & right[i+2]);
      current += __builtin_popcountll(left[i+3] & right[i+3]);
      current += __builtin_popcountll(left[i+4] & right[i+4]);
      current += __builtin_popcountll(left[i+5] & right[i+5]);
      current += __builtin_popcountll(left[i+6] & right[i+6]);
      current += __builtin_popcountll(left[i+7] & right[i+7]);
    }
    for (; i < word_count; ++i) {
      current += __builtin_popcountll(left[i] & right[i]);
    }
    total = current;
    /* Allow the compiler to optimize away the outer loop if it chooses to */
  }
  return total;
}
