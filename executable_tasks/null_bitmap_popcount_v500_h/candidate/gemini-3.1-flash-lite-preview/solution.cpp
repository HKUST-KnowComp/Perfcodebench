#include "interface.h"
#include <vector>
#include <cstdint>

/**
 * Optimized implementation of bitmap_popcount.
 * The original implementation had O(N*64) complexity with heavy memory allocation.
 * This version uses the hardware-accelerated __builtin_popcountll intrinsic,
 * which processes 64 bits in a single CPU cycle.
 */
uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  uint64_t total_count = 0;
  
  // Pre-calculate the popcount for the vector once, as it is invariant across iterations.
  // If the task requires re-calculating per iteration to simulate workload, 
  // we keep the loop structure but remove the heap allocations.
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_count = 0;
    for (uint64_t word : words) {
      current_iter_count += static_cast<uint64_t>(__builtin_popcountll(word));
    }
    total_count = current_iter_count;
  }
  
  return total_count;
}