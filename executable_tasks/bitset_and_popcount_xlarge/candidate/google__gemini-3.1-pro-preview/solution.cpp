#include "interface.h"

#include <cstdint>
#include <vector>

/**
 * Optimized bitset intersection popcount.
 * 
 * The original implementation was extremely slow due to:
 * 1. Dynamic memory allocation (vector.resize) inside the loop.
 * 2. Bit-by-bit extraction and storage into a byte vector.
 * 3. A secondary pass to sum the bytes.
 * 
 * This version uses:
 * 1. __builtin_popcountll: Maps directly to the POPCNT instruction on modern CPUs.
 * 2. Loop Unrolling: Processes 4 words per iteration to improve ILP (Instruction Level Parallelism).
 * 3. Avoids unnecessary allocations and intermediate storage.
 */
uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
  
  const std::size_t n = left.size();
  if (n == 0) return 0;

  uint64_t final_count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_count = 0;
    std::size_t i = 0;

    // Unroll the loop by 4 to maximize throughput of the POPCNT instruction
    // and reduce the overhead of the loop counter and branching.
    for (; i + 3 < n; i += 4) {
      current_count += __builtin_popcountll(left[i] & right[i]);
      current_count += __builtin_popcountll(left[i + 1] & right[i + 1]);
      current_count += __builtin_popcountll(left[i + 2] & right[i + 2]);
      current_count += __builtin_popcountll(left[i + 3] & right[i + 3]);
    }

    // Handle remaining elements
    for (; i < n; ++i) {
      current_count += __builtin_popcountll(left[i] & right[i]);
    }

    final_count = current_count;
  }

  return final_count;
}