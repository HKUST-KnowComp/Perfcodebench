#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty() || key_space == 0 || iters <= 0) {
        return 0;
    }
    // Bit-packed seen vector is extremely cache-efficient for bounded key spaces
    std::vector<bool> seen(key_space, false);
    // Preallocate storage to only reset keys we actually modify, avoiding full key space clears
    std::vector<uint32_t> modified;
    modified.reserve(std::min<size_t>(keys.size(), key_space));
    
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        modified.clear();
        for (uint32_t key : keys) {
            if (!seen[key]) {
                seen[key] = true;
                count++;
                modified.push_back(key);
            }
        }
        // Reset only touched keys for next iteration
        for (uint32_t key : modified) {
            seen[key] = false;
        }
    }
    return count;
}