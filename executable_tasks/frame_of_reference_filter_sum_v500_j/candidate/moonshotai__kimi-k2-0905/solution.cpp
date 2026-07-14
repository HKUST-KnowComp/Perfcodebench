#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters)
{
    const uint16_t* __restrict d = deltas.data();
    const std::size_t n = deltas.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        const uint32_t lo = low;
        const uint32_t hi = high;
        uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
        std::size_t i = 0;

        // unrolled 4×
        for (; i + 3 < n; i += 4) {
            uint32_t v0 = base + static_cast<uint32_t>(d[i+0]);
            uint32_t v1 = base + static_cast<uint32_t>(d[i+1]);
            uint32_t v2 = base + static_cast<uint32_t>(d[i+2]);
            uint32_t v3 = base + static_cast<uint32_t>(d[i+3]);

            sum0 += (v0 >= lo && v0 <= hi) ? v0 : 0;
            sum1 += (v1 >= lo && v1 <= hi) ? v1 : 0;
            sum2 += (v2 >= lo && v2 <= hi) ? v2 : 0;
            sum3 += (v3 >= lo && v3 <= hi) ? v3 : 0;
        }
        total = sum0 + sum1 + sum2 + sum3;

        // tail
        for (; i < n; ++i) {
            uint32_t v = base + static_cast<uint32_t>(d[i]);
            if (v >= lo && v <= hi) total += v;
        }
    }
    return total;
}