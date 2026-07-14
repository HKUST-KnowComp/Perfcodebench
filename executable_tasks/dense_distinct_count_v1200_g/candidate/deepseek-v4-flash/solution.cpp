#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    std::vector<uint32_t> seen(key_space, 0);
    uint32_t gen = 1;
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t cnt = 0;
        for (auto key : keys) {
            if (seen[key] != gen) {
                seen[key] = gen;
                ++cnt;
            }
        }
        count = cnt;
        ++gen;
    }
    return count;
}
