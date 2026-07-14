#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty() || key_space == 0) return 0;

    // Fallback to sorting if key_space is too large for a direct array (~64MB threshold)
    if (key_space > (1u << 24)) {
        std::vector<uint32_t> sorted_keys = keys;
        uint64_t count = 0;
        for (int iter = 0; iter < iters; ++iter) {
            std::sort(sorted_keys.begin(), sorted_keys.end());
            count = static_cast<uint64_t>(std::unique(sorted_keys.begin(), sorted_keys.end()) - sorted_keys.begin());
        }
        return count;
    }

    // Generation array: tracks seen keys using the current iteration number as a marker.
    // Avoids costly memset/clear operations between iterations.
    std::vector<uint32_t> seen(key_space, 0);
    uint32_t* seen_ptr = seen.data();
    uint64_t count = 0;

    for (int iter = 1; iter <= iters; ++iter) {
        count = 0;
        uint32_t marker = static_cast<uint32_t>(iter);
        for (uint32_t key : keys) {
            if (seen_ptr[key] != marker) {
                seen_ptr[key] = marker;
                ++count;
            }
        }
    }
    return count;
}