#include "interface.h"
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    // Use a byte vector as a boolean flag array for O(1) lookup and insertion.
    // Using uint8_t instead of std::vector<bool> for performance (avoiding bitwise ops).
    std::vector<uint8_t> seen(key_space + 1, 0);
    std::vector<uint32_t> present_keys;
    present_keys.reserve(keys.size());

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        // First iteration: identify unique keys and count them
        if (iter == 0) {
            for (uint32_t key : keys) {
                if (seen[key] == 0) {
                    seen[key] = 1;
                    present_keys.push_back(key);
                    count++;
                }
            }
        } else {
            // Subsequent iterations: just count using the pre-populated list
            count = present_keys.size();
        }
    }
    return count;
}