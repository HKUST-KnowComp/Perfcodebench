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
    uint64_t sum = 0;

    // hoist range checks:  value = base + delta
    // low  <= base + delta  <=>  delta >= (low  - base)
    // high >= base + delta  <=>  delta <= (high - base)
    const uint32_t delta_low  = (low  > base) ? (low  - base) : 0u;
    const uint32_t delta_high = (high - base);          // high >= base always valid

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        std::size_t i = 0;

        // 4-way unroll to hide latency and increase ILP
        for (; i + 3 < n; i += 4) {
            uint32_t v0 = base + static_cast<uint32_t>(d[i+0]);
            uint32_t v1 = base + static_cast<uint32_t>(d[i+1]);
            uint32_t v2 = base + static_cast<uint32_t>(d[i+2]);
            uint32_t v3 = base + static_cast<uint32_t>(d[i+3]);

            uint32_t m0 = (d[i+0] >= delta_low  && d[i+0] <= delta_high) ? v0 : 0;
            uint32_t m1 = (d[i+1] >= delta_low  && d[i+1] <= delta_high) ? v1 : 0;
            uint32_t m2 = (d[i+2] >= delta_low  && d[i+2] <= delta_high) ? v2 : 0;
            uint32_t m3 = (d[i+3] >= delta_low  && d[i+3] <= delta_high) ? v3 : 0;

            local_sum += static_cast<uint64_t>(m0) +
                         static_cast<uint64_t>(m1) +
                         static_cast<uint64_t>(m2) +
                         static_cast<uint64_t>(m3);
        }
        // tail
        for (; i < n; ++i) {
            uint32_t delta = d[i];
            if (delta >= delta_low && delta <= delta_high) {
                local_sum += static_cast<uint64_t>(base + delta);
            }
        }
        sum = local_sum;
    }
    return sum;
}