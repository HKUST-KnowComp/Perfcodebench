#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

/**
 * Optimized distinct count using a bitset.
 * Since the keys are bounded by key_space, a bitset is the most efficient way
 * to track seen elements in terms of both time and space complexity.
 */
uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) {
        return 0;
    }

    // Calculate number of 64-bit words needed to represent key_space bits.
    // We use uint64_t to maximize the efficiency of bitwise operations and popcount.
    size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
    std::vector<uint64_t> bitset(num_words, 0);
    
    uint64_t final_count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the bitset for each iteration.
        // std::fill or memset is highly optimized for zeroing memory.
        std::fill(bitset.begin(), bitset.end(), 0);

        // Mark seen keys in the bitset.
        for (uint32_t key : keys) {
            // Safety check for key_space bounds if necessary, 
            // though usually key < key_space is guaranteed.
            bitset[key >> 6] |= (1ULL << (key & 63));
        }

        // Count set bits using the hardware-accelerated popcount instruction.
        uint64_t current_count = 0;
        for (uint64_t word : bitset) {
            current_count += __builtin_popcountll(word);
        }
        final_count = current_count;
    }

    return final_count;
}