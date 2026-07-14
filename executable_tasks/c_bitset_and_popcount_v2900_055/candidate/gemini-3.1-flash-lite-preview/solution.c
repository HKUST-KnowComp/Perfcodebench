#include "interface.h"
#include <stdint.h>

/**
 * Optimized implementation:
 * 1. Removed heap allocation (O(N) space complexity reduced to O(1)).
 * 2. Replaced bit-by-bit extraction with __builtin_popcountll.
 * 3. Eliminated redundant loops, reducing complexity from O(N*64) to O(N).
 */
uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_total = 0;
    for (size_t i = 0; i < word_count; ++i) {
      current_iter_total += (uint64_t)__builtin_popcountll(left[i] & right[i]);
    }
    total = current_iter_total;
  }
  return total;
}