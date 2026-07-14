#include "interface.h"

#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;
    if (iters <= 0) return 0;

    // Use a bitmap to track seen keys. 
    // key_space is the upper bound of the keys.
    size_t bitmap_size = (static_cast<size_t>(key_space) + 63) / 64;
    std::vector<uint64_t> bitmap(bitmap_size, 0);

    uint64_t final_count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitmap for each iteration
        std::memset(bitmap.data(), 0, bitmap_size * sizeof(uint64_t));
        
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