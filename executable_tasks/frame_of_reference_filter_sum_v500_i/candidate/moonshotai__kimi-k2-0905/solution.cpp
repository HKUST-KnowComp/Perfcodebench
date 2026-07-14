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
    const size_t n = deltas.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        size_t i = 0;

        // unroll 4×
        for (; i + 3 < n; i += 4) {
            uint32_t v0 = base + static_cast<uint32_t>(d[i+0]);
            uint32_t v1 = base + static_cast<uint32_t>(d[i+1]);
            uint32_t v2 = base + static_cast<uint32_t>(d[i+2]);
            uint32_t v3 = base + static_cast<uint32_t>(d[i+3]);

            if (v0 >= low && v0 <= high) local += v0;
            if (v1 >= low && v1 <= high) local += v1;
            if (v2 >= low && v2 <= high) local += v2;
            if (v3 >= low && v3 <= high) local += v3;
        }
        // tail
        for (; i < n; ++i) {
            uint32_t v = base + static_cast<uint32_t>(d[i]);
            if (v >= low && v <= high) local += v;
        }
        sum = local;
    }
    return sum;
}