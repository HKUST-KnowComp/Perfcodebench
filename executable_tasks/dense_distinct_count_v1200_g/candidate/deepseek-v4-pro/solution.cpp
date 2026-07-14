#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0 || keys.empty()) {
        return 0;
    }
    std::vector<uint32_t> seen(key_space, 0);
    uint32_t gen = 0;
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        ++gen;
        if (gen == 0) { // wrap-around
            std::fill(seen.begin(), seen.end(), 0);
            gen = 1;
        }
        uint64_t local_count = 0;
        for (uint32_t key : keys) {
            if (seen[key] != gen) {
                seen[key] = gen;
                ++local_count;
            }
        }
        count = local_count;
    }
    return count;
}