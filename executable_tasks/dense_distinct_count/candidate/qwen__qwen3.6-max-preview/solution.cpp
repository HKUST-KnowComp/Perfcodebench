#include "interface.h"

#include <vector>
#include <algorithm>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    // Fallback to sorting if key_space is too large for a direct array
    if (key_space > 64'000'000) {
        std::vector<uint32_t> tmp = keys;
        uint64_t count = 0;
        for (int iter = 0; iter < iters; ++iter) {
            std::sort(tmp.begin(), tmp.end());
            auto last = std::unique(tmp.begin(), tmp.end());
            count = static_cast<uint64_t>(last - tmp.begin());
        }
        return count;
    }

    // Direct addressing with generation counters avoids O(key_space) clear per iteration
    std::vector<uint32_t> seen(key_space, 0);
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint32_t tag = static_cast<uint32_t>(iter + 1);
        count = 0;
        for (uint32_t key : keys) {
            if (seen[key] != tag) {
                seen[key] = tag;
                ++count;
            }
        }
    }
    return count;
}