#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Optimized bitset intersection popcount.
 * 
 * The original implementation was extremely inefficient, allocating memory and 
 * manually extracting bits into a byte array before summing them.
 * 
 * This version uses the __builtin_popcountll intrinsic which maps to the 
 * POPCNT instruction on modern x86_64 and ARM architectures, providing 
 * a massive speedup.
 */
uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;

    // The outer loop is required by the harness to measure performance over iterations.
    for (int iter = 0; iter < iters; ++iter) {
        total = 0;
        // Process words in blocks to allow for better instruction pipelining.
        // Compilers will likely unroll this loop and use SIMD if available.
        for (size_t i = 0; i < word_count; ++i) {
            uint64_t intersection = left[i] & right[i];
            total += (uint64_t)__builtin_popcountll(intersection);
        }
    }

    return total;
}