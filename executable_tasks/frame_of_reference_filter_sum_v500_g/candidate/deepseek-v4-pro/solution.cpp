#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    uint64_t final_sum = 0;
    const std::size_t n = deltas.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            uint32_t value = base + static_cast<uint32_t>(deltas[i]);
            if (value >= low && value <= high) {
                sum += value;
            }
        }
        final_sum = sum;
    }
    return final_sum;
}
