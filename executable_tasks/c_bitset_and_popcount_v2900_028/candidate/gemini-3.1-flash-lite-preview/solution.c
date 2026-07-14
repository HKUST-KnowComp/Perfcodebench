#include "interface.h"
#include <stdint.h>

/**
 * Optimized implementation:
 * The original implementation was performing an unnecessary O(N*64) expansion into a byte array.
 * We can compute the popcount of the AND result directly using the __builtin_popcountll 
 * intrinsic, which maps to the POPCNT instruction on modern x86_64 CPUs.
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