#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>
#include <algorithm>

/**
 * Optimized distinct count for bounded keys.
 * Uses a bitset (array of uint64_t) to track seen keys.
 * This is significantly faster than std::unordered_set due to:
 * 1. Better cache locality (1 bit per key).
 * 2. No heap allocations per insertion.
 * 3. Fast clearing via memset.
 * 4. Hardware-accelerated counting via __builtin_popcountll.
 */
uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;

    // Calculate number of 64-bit words needed to represent key_space bits.
    // key_space is the upper bound (exclusive), so we need bits [0, key_space-1].
    size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
    
    // Allocate bitset buffer.
    std::vector<uint64_t> bitset(num_words);
    uint64_t final_count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitset for each iteration.
        std::memset(bitset.data(), 0, num_words * sizeof(uint64_t));

        // Mark seen keys.
        // Note: We assume keys[i] < key_space as per the 'bounded-key' description.
        for (uint32_t key : keys) {
            bitset[key >> 6] |= (1ULL << (key & 63));
        }

        // Count set bits using hardware-accelerated popcount.
        uint64_t current_count = 0;
        for (size_t i = 0; i < num_words; ++i) {
            current_count += __builtin_popcountll(bitset[i]);
        }
        final_count = current_count;
    }

    return final_count;
}