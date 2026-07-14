#include "interface.h"
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty()) return 0;
    
    // Use a byte vector as a boolean flag array for O(1) access.
    // Using uint8_t instead of std::vector<bool> for performance.
    std::vector<uint8_t> seen(key_space + 1, 0);
    std::vector<uint32_t> present_keys;
    present_keys.reserve(keys.size());

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        // On the first iteration, identify unique keys to avoid re-scanning the full key_space
        if (iter == 0) {
            for (uint32_t key : keys) {
                if (seen[key] == 0) {
                    seen[key] = 1;
                    present_keys.push_back(key);
                    count++;
                }
            }
        } else {
            // Subsequent iterations: only clear the keys we know were set
            for (uint32_t key : present_keys) {
                seen[key] = 0;
            }
            for (uint32_t key : keys) {
                if (seen[key] == 0) {
                    seen[key] = 1;
                    count++;
                }
            }
        }
    }
    return count;
}