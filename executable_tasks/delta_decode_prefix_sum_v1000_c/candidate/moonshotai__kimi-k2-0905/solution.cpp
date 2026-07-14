#include "interface.h"
#include <cstddef>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
    const std::size_t n = deltas.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t value = base;
        uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
        std::size_t i = 0;

        // 4-way unrolled scalar loop
        for (; i + 3 < n; i += 4) {
            value += static_cast<uint64_t>(deltas[i]);
            sum0 += value;
            value += static_cast<uint64_t>(deltas[i+1]);
            sum1 += value;
            value += static_cast<uint64_t>(deltas[i+2]);
            sum2 += value;
            value += static_cast<uint64_t>(deltas[i+3]);
            sum3 += value;
        }
        total = sum0 + sum1 + sum2 + sum3;

        // tail
        for (; i < n; ++i) {
            value += static_cast<uint64_t>(deltas[i]);
            total += value;
        }
    }
    return total;
}