#include "interface.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Optimized implementation of bitset intersection popcount.
 * 
 * The baseline implementation was extremely inefficient, allocating memory and 
 * expanding every bit into a byte array before summing. 
 * 
 * This version uses the __builtin_popcountll intrinsic, which typically maps to 
 * the POPCNT instruction on modern CPUs (x86_64 with SSE4.2+ or ARMv8).
 */
uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
    uint64_t total = 0;

    // The outer loop is required by the harness to measure performance over iterations.
    for (int iter = 0; iter < iters; ++iter) {
        total = 0;
        // Process the bitsets word by word (64 bits at a time).
        for (size_t i = 0; i < word_count; ++i) {
            // Compute the bitwise AND of the two words.
            uint64_t intersection = left[i] & right[i];
            // Use the compiler intrinsic for population count (number of set bits).
            total += (uint64_t)__builtin_popcountll(intersection);
        }
    }

    return total;
}