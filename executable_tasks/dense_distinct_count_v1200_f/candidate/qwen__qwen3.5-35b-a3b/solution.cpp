#include "interface.h"

#include <vector>
#include <cstdint>
#include <algorithm>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (iters <= 0 || keys.empty()) {
        return 0;
    }

    // Since keys is const, the result is invariant across iterations.
    // We compute once to save time.
    uint64_t count = 0;

    // Threshold for bitmap vs sorting. 10M bytes is ~10MB, safe for most systems.
    const uint32_t BITMAP_THRESHOLD = 10000000;

    if (key_space <= BITMAP_THRESHOLD && key_space > 0) {
        // Use bitmap for O(N) performance with low constant
        std::vector<uint8_t> seen(key_space, 0);
        for (uint32_t key : keys) {
            if (key < key_space && !seen[key]) {
                seen[key] = 1;
                ++count;
            }
        }
    } else {
        // Use sorting for O(N log N) performance when key_space is too large
        std::vector<uint32_t> sorted_keys = keys;
        std::sort(sorted_keys.begin(), sorted_keys.end());
        
        if (!sorted_keys.empty()) {
            count = 1;
            for (size_t i = 1; i < sorted_keys.size(); ++i) {
                if (sorted_keys[i] != sorted_keys[i - 1]) {
                    ++count;
                }
            }
        }
    }

    return count;
}