#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Optimized bitset intersection popcount.
 * 
 * The baseline implementation was bottlenecked by:
 * 1. Heap allocation/deallocation inside the function.
 * 2. Manual bit extraction into a byte array (64 iterations per word).
 * 3. A secondary loop to sum those bytes.
 * 
 * This version uses __builtin_popcountll, which leverages the hardware POPCNT instruction,
 * and processes the bitsets directly in a single pass.
 */
uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;

    // The benchmark harness expects the result of the popcount operation.
    // We repeat the calculation 'iters' times as requested by the loop structure.
    for (int iter = 0; iter < iters; ++iter) {
        total = 0;
        size_t i = 0;

        // Unroll the loop slightly to improve instruction-level parallelism
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