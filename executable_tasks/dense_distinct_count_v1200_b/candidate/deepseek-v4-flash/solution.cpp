#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0) return 0;
    std::vector<uint32_t> seen(key_space, 0);
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t cur_count = 0;
        uint32_t mark = iter + 1;
        for (uint32_t key : keys) {
            if (seen[key] != mark) {
                seen[key] = mark;
                ++cur_count;
            }
        }
        count = cur_count;
    }
    return count;
}
