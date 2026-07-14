#include "interface.h"
#include <cstdint>
#include <cstddef>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters)
{
    uint64_t sum = 0;
    const uint16_t* __restrict d = deltas.data();
    const size_t n = deltas.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        for (size_t i = 0; i < n; ++i) {
            uint32_t v = base + static_cast<uint32_t>(d[i]);
            uint32_t in_range = (v >= low) & (v <= high);
            local += static_cast<uint64_t>(v) * in_range;
        }
        sum = local;
    }
    return sum;
}