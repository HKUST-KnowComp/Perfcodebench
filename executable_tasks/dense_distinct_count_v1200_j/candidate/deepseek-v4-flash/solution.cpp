#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    if (key_space == 0) return 0;
    std::vector<uint32_t> stamps(key_space, 0);
    uint32_t cur_stamp = 0;
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        ++cur_stamp;
        uint64_t local_count = 0;
        for (uint32_t key : keys) {
            if (stamps[key] != cur_stamp) {
                stamps[key] = cur_stamp;
                ++local_count;
            }
        }
        count = local_count;
    }
    return count;
}
