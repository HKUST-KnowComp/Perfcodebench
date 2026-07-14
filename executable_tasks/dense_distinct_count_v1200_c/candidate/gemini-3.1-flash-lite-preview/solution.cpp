#include "interface.h"
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    // Use a bitset to track seen keys. 
    // Size is (key_space + 63) / 64 to store bits in uint64_t array.
    size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
    std::vector<uint64_t> seen(num_words, 0);
    std::vector<uint32_t> touched_indices;
    touched_indices.reserve(keys.size());

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        for (uint32_t key : keys) {
            uint32_t word_idx = key >> 6;
            uint64_t bit_mask = 1ULL << (key & 63);
            
            if (!(seen[word_idx] & bit_mask)) {
                seen[word_idx] |= bit_mask;
                touched_indices.push_back(word_idx);
                count++;
            }
        }
        
        // Reset only the modified parts of the bitset for efficiency
        for (uint32_t idx : touched_indices) {
            seen[idx] = 0;
        }
        touched_indices.clear();
    }
    return count;
}