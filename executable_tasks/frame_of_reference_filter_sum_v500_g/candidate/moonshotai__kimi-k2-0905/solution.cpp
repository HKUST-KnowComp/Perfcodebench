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
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        for (std::size_t i = 0; i < deltas.size(); ++i) {
            uint32_t v = base + static_cast<uint32_t>(deltas[i]);
            if (v >= low && v <= high)
                local += static_cast<uint64_t>(v);
        }
        sum = local;
    }
    return sum;
}