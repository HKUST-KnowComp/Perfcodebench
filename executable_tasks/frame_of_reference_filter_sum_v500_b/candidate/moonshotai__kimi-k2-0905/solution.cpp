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
    const std::size_t n = deltas.size();
    const uint32_t lo = low;
    const uint32_t hi = high;
    const uint32_t bias = base;

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t acc = 0;
        std::size_t i = 0;

        // 4× unrolled main loop
        for (; i + 3 < n; i += 4) {
            uint32_t v0 = bias + static_cast<uint32_t>(deltas[i + 0]);
            uint32_t v1 = bias + static_cast<uint32_t>(deltas[i + 1]);
            uint32_t v2 = bias + static_cast<uint32_t>(deltas[i + 2]);
            uint32_t v3 = bias + static_cast<uint32_t>(deltas[i + 3]);

            if (v0 >= lo && v0 <= hi) acc += v0;
            if (v1 >= lo && v1 <= hi) acc += v1;
            if (v2 >= lo && v2 <= hi) acc += v2;
            if (v3 >= lo && v3 <= hi) acc += v3;
        }

        // Remaining elements
        for (; i < n; ++i) {
            uint32_t v = bias + static_cast<uint32_t>(deltas[i]);
            if (v >= lo && v <= hi) acc += v;
        }

        sum = acc;
    }

    return sum;
}