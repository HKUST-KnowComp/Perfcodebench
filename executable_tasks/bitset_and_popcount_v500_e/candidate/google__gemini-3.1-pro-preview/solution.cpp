#include "interface.h"

#include <cstdint>
#include <vector>

/**
 * Optimized bitset intersection popcount.
 * 
 * The baseline implementation was extremely inefficient, expanding each bit into a byte 
 * in a temporary vector and then summing those bytes. 
 * 
 * This version uses the `__builtin_popcountll` intrinsic, which maps directly to the 
 * hardware POPCNT instruction on modern x86 and ARM CPUs. We also unroll the loop 
 * to improve instruction-level parallelism (ILP) and reduce loop overhead.
 */
uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  
  const std::size_t n = left.size();
  uint64_t total_count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    total_count = 0;
    std::size_t i = 0;

    // Unroll the loop by 4 to process multiple words per iteration
    // and maximize throughput of the POPCNT instruction.
    for (; i + 3 < n; i += 4) {
      uint64_t c0 = left[i] & right[i];
      uint64_t c1 = left[i + 1] & right[i + 1];
      uint64_t c2 = left[i + 2] & right[i + 2];
      uint64_t c3 = left[i + 3] & right[i + 3];

      total_count += __builtin_popcountll(c0);
      total_count += __builtin_popcountll(c1);
      total_count += __builtin_popcountll(c2);
      total_count += __builtin_popcountll(c3);
    }

    // Handle remaining elements
    for (; i < n; ++i) {
      total_count += __builtin_popcountll(left[i] & right[i]);
    }
  }

  return total_count;
}