#include "interface.h"
#include <vector>
#include <cstring>
#include <algorithm>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;
    if (key_space == 0) return 0;

    // Use a bitset to track seen keys. 
    // size = ceil(key_space / 64)
    size_t num_words = (key_space + 63) / 64;
    std::vector<uint64_t> bitset(num_words, 0);

    uint64_t final_count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitset for each iteration
        std::fill(bitset.begin(), bitset.end(), 0);
        
        for (uint32_t key : keys) {
            // Safety check for key_space boundary
            if (key < key_space) {
                bitset[key >> 6] |= (1ULL << (key & 63));
            }
        }

        uint64_t current_count = 0;
        for (uint64_t word : bitset) {
            // __builtin_popcountll is a fast intrinsic for counting set bits
            current_count += __builtin_popcountll(word);
        }
        final_count = current_count;
    }

    return final_count;
}