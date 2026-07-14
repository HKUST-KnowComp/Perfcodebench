#include "interface.h"

#include <cstdint>
#include <vector>

/**
 * Optimized bitset_and_count implementation.
 * 
 * The original implementation was extremely inefficient, expanding each bit into a byte
 * in a temporary vector before summing them. This version uses the __builtin_popcountll
 * intrinsic, which maps directly to the hardware POPCNT instruction on modern CPUs.
 * 
 * Performance improvements:
 * 1. Replaced O(N * 64) bit-extraction loop with O(N) word-level popcount.
 * 2. Eliminated heap allocations (vector::resize) inside the iteration loop.
 * 3. Leverages hardware-level parallelism for bit counting.
 */
uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  
  uint64_t total_count = 0;
  const std::size_t size = left.size();
  const uint64_t* l_ptr = left.data();
  const uint64_t* r_ptr = right.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_count = 0;
    // Unrolling or SIMD could be used, but __builtin_popcountll with 
    // compiler optimization is typically sufficient to saturate execution ports.
    for (std::size_t i = 0; i < size; ++i) {
      current_iter_count += __builtin_popcountll(l_ptr[i] & r_ptr[i]);
    }
    total_count = current_iter_count;
  }

  return total_count;
}