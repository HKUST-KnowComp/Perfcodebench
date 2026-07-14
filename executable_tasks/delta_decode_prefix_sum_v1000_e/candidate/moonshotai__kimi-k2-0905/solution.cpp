#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const uint32_t* d = deltas.data();
    const std::size_t n = deltas.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t value = base;
        uint64_t local = 0;
        std::size_t i = 0;

        // 4-way unrolled main loop
        for (; i + 3 < n; i += 4) {
            value += static_cast<uint64_t>(d[i]);
            local += value;
            value += static_cast<uint64_t>(d[i+1]);
            local += value;
            value += static_cast<uint64_t>(d[i+2]);
            local += value;
            value += static_cast<uint64_t>(d[i+3]);
            local += value;
        }
        // tail
        for (; i < n; ++i) {
            value += static_cast<uint64_t>(d[i]);
            local += value;
        }
        total = local;  // overwrite each iteration as required
    }
    return total;
}