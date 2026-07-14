#include "interface.h"
#include <vector>
#include <cstdint>

/**
 * Optimized bitmap popcount implementation.
 * 
 * The original code expanded bits into a temporary vector of bytes, which is extremely slow
 * due to memory allocation, cache pressure, and redundant operations.
 * 
 * This version uses the __builtin_popcountll intrinsic, which leverages the hardware POPCNT
 * instruction available on modern processors (x86 SSE4.2+ or ARM Neon).
 */
uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
  uint64_t total_count = 0;
  
  // The benchmark harness expects the calculation to be repeated 'iters' times.
  for (int iter = 0; iter < iters; ++iter) {
    total_count = 0;
    
    // Use a pointer-based loop or range-based for loop for efficiency.
    // Modern compilers will unroll this loop and potentially use SIMD instructions
    // if the target architecture supports vectorized popcount.
    for (uint64_t word : words) {
      total_count += static_cast<uint64_t>(__builtin_popcountll(word));
    }
  }
  
  return total_count;
}