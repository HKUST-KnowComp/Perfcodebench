#include "interface.h"
#include <vector>
#include <cstring>
#include <algorithm>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;
    if (iters <= 0) return 0;

    // Use a bitset to track seen keys. 
    // key_space is the upper bound of the keys.
    size_t num_words = (key_space + 63) / 64;
    std::vector<uint64_t> bitset(num_words, 0);

    uint64_t final_count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Fast clear of the bitset
        std::memset(bitset.data(), 0, num_words * sizeof(uint64_t));
        
        uint64_t current_distinct = 0;
        for (uint32_t key : keys) {
            // Only process keys within the specified key_space to avoid OOB
            if (key < key_space) {
                uint64_t word_idx = key >> 6;
                uint64_t bit_mask = 1ULL << (key & 63);
                if (!(bitset[word_idx] & bit_mask)) {
                    bitset[word_idx] |= bit_mask;
                    current_distinct++;
                }
            }
        }
        final_count = current_distinct;
    }

    return final_count;
}