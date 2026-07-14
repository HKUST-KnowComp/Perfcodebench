#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;

    // Use a bitset to track seen keys. 
    // A vector<bool> is specialized in C++ to be space-efficient (1 bit per element).
    // We pre-allocate the size based on key_space to avoid reallocations.
    std::vector<bool> seen(key_space, false);
    uint64_t final_count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the bitset for each iteration
        std::fill(seen.begin(), seen.end(), false);
        
        for (uint32_t key : keys) {
            // Safety check to ensure key is within bounds, though task implies bounded keys
            if (key < key_space) {
                seen[key] = true;
            }
        }

        // Count the number of set bits
        uint64_t current_count = 0;
        for (bool val : seen) {
            if (val) current_count++;
        }
        final_count = current_count;
    }

    return final_count;
}