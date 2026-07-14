#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;

    // Use a bitset approach for O(1) access and minimal memory footprint.
    // We use a vector of uint64_t to act as a manual bitset.
    size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
    std::vector<uint64_t> bitset(num_words, 0);
    uint64_t final_count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitset
        for (size_t i = 0; i < num_words; ++i) {
            bitset[i] = 0;
        }

        // Mark keys
        for (uint32_t key : keys) {
            // Safety check against key_space if necessary, though task implies keys < key_space
            if (key < key_space) {
                bitset[key >> 6] |= (1ULL << (key & 63));
            }
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