#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    std::vector<uint32_t> seen(key_space, 0);
    uint64_t count = 0;
    for (int iter = 1; iter <= iters; ++iter) {
        count = 0;
        uint32_t tag = static_cast<uint32_t>(iter);
        for (uint32_t key : keys) {
            if (seen[key] != tag) {
                seen[key] = tag;
                ++count;
            }
        }
    }
    return count;
}
