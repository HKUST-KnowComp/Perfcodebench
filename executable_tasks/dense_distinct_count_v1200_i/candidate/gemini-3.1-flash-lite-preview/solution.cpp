#include "interface.h"
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    // Use a byte array as a boolean flag array to track seen keys.
    // This is significantly faster than std::unordered_set for bounded integer keys.
    std::vector<uint8_t> seen(key_space + 1, 0);
    uint64_t count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset the tracking array for each iteration.
        std::memset(seen.data(), 0, seen.size());
        uint64_t current_count = 0;

        for (uint32_t key : keys) {
            if (key <= key_space && seen[key] == 0) {
                seen[key] = 1;
                current_count++;
            }
        }
        count = current_count;
    }
    return count;
}