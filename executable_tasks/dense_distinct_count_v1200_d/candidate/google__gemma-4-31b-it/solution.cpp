#include "interface.h"

#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;
    if (iters <= 0) return 0;

    // Use a bitset to track seen keys. 
    // key_space is the upper bound of the keys.
    size_t num_bits = static_cast<size_t>(key_space) + 1;
    size_t num_words = (num_bits + 63) / 64;
    std::vector<uint64_t> bitset(num_words, 0);

    uint64_t final_count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitset efficiently
        std::memset(bitset.data(), 0, num_words * sizeof(uint64_t));
        
        for (uint32_t key : keys) {
            bitset[key >> 6] |= (1ULL << (key & 63));
        }

        uint64_t current_count = 0;
        for (size_t i = 0; i < num_words; ++i) {
            current_count += __builtin_popcountll(bitset[i]);
        }
        final_count = current_count;
    }

    return final_count;
}