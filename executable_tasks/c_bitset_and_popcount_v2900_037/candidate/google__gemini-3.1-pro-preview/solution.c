#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized implementation of bitset intersection popcount.
 * 
 * The baseline implementation was extremely inefficient, performing manual bit extraction
 * into a heap-allocated buffer and then summing the bytes. 
 * 
 * Improvements:
 * 1. Replaced manual bit manipulation with __builtin_popcountll, which maps to the 
 *    hardware POPCNT instruction on modern CPUs.
 * 2. Eliminated malloc/free overhead.
 * 3. Applied loop unrolling (factor of 4) to improve instruction-level parallelism.
 * 4. Maintained the logic where 'total' is reset each iteration, matching the baseline's behavior.
 */
uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    total = 0;
    size_t i = 0;

    // Unroll the loop to process 4 words at a time for better throughput
    for (; i + 3 < word_count; i += 4) {
      uint64_t c0 = left[i] & right[i];
      uint64_t c1 = left[i + 1] & right[i + 1];
      uint64_t c2 = left[i + 2] & right[i + 2];
      uint64_t c3 = left[i + 3] & right[i + 3];

      total += (uint64_t)__builtin_popcountll(c0);
      total += (uint64_t)__builtin_popcountll(c1);
      total += (uint64_t)__builtin_popcountll(c2);
      total += (uint64_t)__builtin_popcountll(c3);
    }

    // Handle remaining words
    for (; i < word_count; ++i) {
      total += (uint64_t)__builtin_popcountll(left[i] & right[i]);
    }
  }

  return total;
}