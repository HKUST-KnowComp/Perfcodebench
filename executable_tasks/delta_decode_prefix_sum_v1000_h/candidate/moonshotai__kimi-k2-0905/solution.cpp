#include "interface.h"
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const std::size_t n = deltas.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t value = base;
        uint64_t local = 0;
        const uint32_t* __restrict src = deltas.data();

        std::size_t i = 0;
        // process 4 elements per iteration
        for (; i + 3 < n; i += 4) {
            value += static_cast<uint64_t>(src[i]);
            local += value;
            value += static_cast<uint64_t>(src[i + 1]);
            local += value;
            value += static_cast<uint64_t>(src[i + 2]);
            local += value;
            value += static_cast<uint64_t>(src[i + 3]);
            local += value;
        }
        // handle remaining elements
        for (; i < n; ++i) {
            value += static_cast<uint64_t>(src[i]);
            local += value;
        }
        sum = local;
    }
    return sum;
}