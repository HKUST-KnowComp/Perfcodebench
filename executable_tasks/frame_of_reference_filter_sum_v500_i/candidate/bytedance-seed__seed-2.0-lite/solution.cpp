#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    const uint32_t min_delta = (low > base) ? (low - base) : 0U;
    const uint32_t max_delta = (high >= base) ? (high - base) : 0xFFFFFFFFU;
    
    if (min_delta > max_delta || base + 0xFFFFU < low || base > high) {
        uint64_t sum = 0;
        for (int iter = 0; iter < iters; ++iter) {
            sum = 0;
        }
        return sum;
    }

    uint64_t sum = 0;
    const uint16_t* delta_ptr = deltas.data();
    const size_t delta_count = deltas.size();

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < delta_count; ++i) {
            const uint32_t val = base + static_cast<uint32_t>(delta_ptr[i]);
            if (val >= low && val <= high) {
                sum += val;
            }
        }
    }
    return sum;
}