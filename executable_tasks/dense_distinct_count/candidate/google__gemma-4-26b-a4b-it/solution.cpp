#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;
    
    // Use a bitset approach for O(1) insertion and minimal memory footprint.
    // We use a vector of uint64_t to act as a manual bitset for maximum speed.
    uint64_t count = 0;
    uint64_t num_words = (static_cast<uint64_t>(key_space) + 63) / 64;
    std::vector<uint64_t> bitset(num_words, 0);

    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitset for each iteration
        std::fill(bitset.begin(), bitset.end(), 0);
        
        for (uint32_t key : keys) {
            // Only process keys within the valid key_space
            if (key < key_space) {
                bitset[key >> 6] |= (1ULL << (key & 63));
            }
        }

        // Count set bits
        uint64_t current_count = 0;
        for (uint64_t word : bitset) {
            // __builtin_popcountll is a highly optimized intrinsic for counting set bits
            current_count += static_cast<uint64_t>(__builtin_popcountll(word));
        }
        count = current_count;
    }

    return count;
}