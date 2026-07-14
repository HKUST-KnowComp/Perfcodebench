#include "interface.h"
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;

    // Calculate number of 64-bit words needed to represent key_space bits
    size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
    std::vector<uint64_t> bitset(num_words, 0);
    uint64_t final_count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitset for each iteration
        std::memset(bitset.data(), 0, num_words * sizeof(uint64_t));

        // Mark seen keys
        for (uint32_t key : keys) {
            // Safety check for key_space bounds if necessary, 
            // though the problem implies keys < key_space.
            bitset[key >> 6] |= (1ULL << (key & 63));
        }

        // Count set bits
        uint64_t current_count = 0;
        for (size_t i = 0; i < num_words; ++i) {
            current_count += __builtin_popcountll(bitset[i]);
        }
        final_count = current_count;
    }

    return final_count;
}