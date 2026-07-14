#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) {
        return 0;
    }

    // Calculate the number of 64-bit words needed to represent the key space.
    // key_space is the upper bound (exclusive) for the keys.
    size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
    if (num_words == 0) return 0;

    // Pre-allocate bitset memory to reuse across iterations.
    std::vector<uint64_t> bitset(num_words, 0);
    uint64_t* bitset_ptr = bitset.data();
    uint64_t final_count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the bitset for each iteration.
        std::fill(bitset.begin(), bitset.end(), 0);
        uint64_t current_count = 0;

        for (uint32_t key : keys) {
            // Safety check for key_space bounds, though usually keys < key_space.
            if (key >= key_space) continue;

            uint32_t word_idx = key >> 6;      // key / 64
            uint64_t bit_mask = 1ULL << (key & 63); // key % 64

            // If the bit is not set, it's a new distinct key.
            if (!(bitset_ptr[word_idx] & bit_mask)) {
                bitset_ptr[word_idx] |= bit_mask;
                current_count++;
            }
        }
        final_count = current_count;
    }

    return final_count;
}