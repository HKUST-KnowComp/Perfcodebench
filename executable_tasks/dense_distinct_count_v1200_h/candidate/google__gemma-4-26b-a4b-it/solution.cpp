#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) {
        return 0;
    }

    // Use a bitset implemented via a vector of uint64_t for maximum performance.
    // Size is (key_space + 63) / 64 to cover all possible keys.
    size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
    std::vector<uint64_t> bitset(num_words, 0);
    
    uint64_t final_count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitset using memset for speed
        std::memset(bitset.data(), 0, num_words * sizeof(uint64_t));

        for (uint32_t key : keys) {
            // Only process keys within the valid range
            if (key < key_space) {
                bitset[key >> 6] |= (1ULL << (key & 63));
            }
        }

        // Count set bits
        uint64_t current_count = 0;
        for (size_t i = 0; i < num_words; ++i) {
            // __builtin_popcountll is a highly optimized intrinsic for counting set bits
            current_count += static_cast<uint64_t>(__builtin_popcountll(bitset[i]));
        }
        final_count = current_count;
    }

    return final_count;
}