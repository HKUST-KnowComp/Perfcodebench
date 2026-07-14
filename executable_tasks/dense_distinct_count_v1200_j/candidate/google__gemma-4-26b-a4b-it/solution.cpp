#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;

    // Use a bitset implemented via a vector of uint64_t for maximum performance.
    // Size is (key_space + 63) / 64 to ensure all possible keys are covered.
    size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
    std::vector<uint64_t> bitset(num_words);
    uint64_t final_count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitset for each iteration
        std::fill(bitset.begin(), bitset.end(), 0ULL);

        for (uint32_t key : keys) {
            // Only process keys within the valid key_space
            if (key < key_space) {
                bitset[key >> 6] |= (1ULL << (key & 63));
            }
        }

        // Count set bits (population count)
        uint64_t current_count = 0;
        for (uint64_t word : bitset) {
            // __builtin_popcountll is a compiler intrinsic for the hardware POPCNT instruction
            current_count += static_cast<uint64_t>(__builtin_popcountll(word));
        }
        final_count = current_count;
    }

    return final_count;
}