#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const std::size_t n = deltas.size();
        const uint16_t* data = deltas.data();
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t val = base + static_cast<uint32_t>(data[i]);
            if (val >= low && val <= high) {
                sum += static_cast<uint64_t>(val);
            }
        }
    }
    return sum;
}
