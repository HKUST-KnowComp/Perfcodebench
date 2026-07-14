#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (keys.empty() || key_space == 0) return 0;
    std::vector<uint32_t> seen(key_space, 0);
    uint64_t final_count = 0;
    for (int iter = 1; iter <= iters; ++iter) {
        uint64_t count = 0;
        for (uint32_t key : keys) {
            if (seen[key] != iter) {
                count++;
                seen[key] = iter;
            }
        }
        final_count = count;
    }
    return final_count;
}