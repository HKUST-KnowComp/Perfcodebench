#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    uint64_t total = 0;
    const std::size_t n = deltas.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t partial = 0;
        uint64_t value = base;
        const uint32_t* __restrict src = deltas.data();

        for (std::size_t i = 0; i < n; ++i) {
            value += static_cast<uint64_t>(src[i]);
            partial += value;
        }
        total = partial;
    }
    return total;
}