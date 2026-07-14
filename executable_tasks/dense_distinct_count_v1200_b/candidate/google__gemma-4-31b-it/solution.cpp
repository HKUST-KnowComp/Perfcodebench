#include "interface.h"

#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;

    // Use a bitmap to track seen keys. 
    // key_space is the upper bound of the keys.
    // We need (key_space + 1) bits.
    size_t num_words = (static_cast<size_t>(key_space) + 64) / 64;
    std::vector<uint64_t> bitmap(num_words);

    uint64_t final_count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitmap efficiently
        std::memset(bitmap.data(), 0, num_words * sizeof(uint64_t));
        
        uint64_t current_count = 0;
        for (uint32_t key : keys) {
            uint64_t word_idx = key >> 6; // key / 64
            uint64_t bit_mask = 1ULL << (key & 63); // key % 64
            
            if (!(bitmap[word_idx] & bit_mask)) {
                bitmap[word_idx] |= bit_mask;
                current_count++;
            }
        }
        final_count = current_count;
    }

    return final_count;
}