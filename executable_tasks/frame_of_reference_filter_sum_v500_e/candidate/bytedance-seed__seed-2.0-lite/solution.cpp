#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    const std::size_t n = deltas.size();
    const uint16_t* const d_data = deltas.data();
    const uint32_t base_val = base;
    const uint32_t min_val = low;
    const uint32_t max_val = high;
    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (std::size_t i = 0; i < n; ++i) {
            const uint32_t val = base_val + static_cast<uint32_t>(d_data[i]);
            if (val >= min_val && val <= max_val) {
                sum += val;
            }
        }
        final_sum = sum;
    }
    return final_sum;
}