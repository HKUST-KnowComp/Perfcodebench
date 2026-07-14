#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized implementation of bitset intersection popcount.
 * 
 * Improvements:
 * 1. Removed malloc/free: The original code allocated a massive byte array unnecessarily.
 * 2. Replaced manual bit extraction: Used __builtin_popcountll which maps to the hardware POPCNT instruction.
 * 3. Loop Unrolling: Unrolled the inner loop to process multiple words per iteration, 
 *    improving instruction-level parallelism (ILP) and reducing loop overhead.
 * 4. Eliminated redundant operations: Directly computed the AND and popcount in one pass.
 */
uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_total = 0;
        size_t i = 0;

        // Unroll the loop by 4 to leverage ILP
        for (; i + 3 < word_count; i += 4) {
            uint64_t c0 = __builtin_popcountll(left[i] & right[i]);
            uint64_t c1 = __builtin_popcountll(left[i + 1] & right[i + 1]);
            uint64_t c2 = __builtin_popcountll(left[i + 2] & right[i + 2]);
            uint64_t c3 = __builtin_popcountll(left[i + 3] & right[i + 3]);
            current_total += (c0 + c1 + c2 + c3);
        }

        // Handle remaining words
        for (; i < word_count; ++i) {
            current_total += __builtin_popcountll(left[i] & right[i]);
        }

        total = current_total;
    }

    return total;
}