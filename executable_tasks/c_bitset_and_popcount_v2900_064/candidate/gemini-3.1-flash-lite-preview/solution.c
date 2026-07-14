#include "interface.h"
#include <stdint.h>

/**
 * Optimized implementation using __builtin_popcountll.
 * The original implementation was O(N*64) due to bit-by-bit extraction 
 * and heap allocation. This version is O(N) and cache-friendly.
 */
uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_total = 0;
    for (size_t i = 0; i < word_count; ++i) {
      // Perform bitwise AND and count set bits using hardware intrinsic
      current_iter_total += (uint64_t)__builtin_popcountll(left[i] & right[i]);
    }
    total = current_iter_total;
  }
  return total;
}