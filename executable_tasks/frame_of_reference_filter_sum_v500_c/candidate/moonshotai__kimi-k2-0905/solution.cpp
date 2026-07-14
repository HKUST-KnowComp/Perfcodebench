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
    const uint64_t b = base;
    const uint64_t lo = low;
    const uint64_t hi = high;
    const size_t n = deltas.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        for (size_t i = 0; i < n; ++i) {
            const uint64_t v = b + deltas[i];
            if (v >= lo && v <= hi)
                local += v;
        }
        sum = local;
    }
    return sum;
}