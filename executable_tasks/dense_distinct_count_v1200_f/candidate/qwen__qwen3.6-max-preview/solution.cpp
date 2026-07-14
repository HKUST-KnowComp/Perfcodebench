#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0 || keys.empty()) return 0;

    // Generation array: stores the last iteration tag that saw each key.
    // Avoids O(key_space) memset/clear on every iteration.
    std::vector<uint32_t> seen(key_space, 0);
    uint64_t count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        uint32_t tag = static_cast<uint32_t>(iter) + 1;
        for (uint32_t key : keys) {
            if (seen[key] != tag) {
                seen[key] = tag;
                ++count;
            }
        }
    }
    return count;
}
