#include "interface.h"
#include <vector>
#include <cstring>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters)
{
    std::vector<uint8_t> bitmap(key_space, 0);
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        if (iter == 0) {
            std::fill(bitmap.begin(), bitmap.end(), uint8_t(0));
            for (uint32_t k : enabled_keys)
                bitmap[k] = 1;

            const size_t n = probe_keys.size();
            const uint32_t* __restrict pk = probe_keys.data();
            const uint32_t* __restrict pv = values.data();
            uint8_t* __restrict bm = bitmap.data();
            uint64_t local = 0;
            for (size_t i = 0; i < n; ++i)
                local += uint64_t(pv[i]) * bm[pk[i]];
            sum = local;
        }
    }
    return sum;
}