#include "interface.h"
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    uint64_t sum = 0;
    const size_t n = deltas.size();
    const uint16_t* const d_data = deltas.data();
    // Cache constant parameters in registers for low-latency access
    const uint32_t c_base = base;
    const uint32_t c_low = low;
    const uint32_t c_high = high;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        // Single pass decode + sum to eliminate intermediate storage overhead
        for (size_t i = 0; i < n; ++i) {
            const uint32_t val = c_base + static_cast<uint32_t>(d_data[i]);
            if (val >= c_low && val <= c_high) {
                sum += static_cast<uint64_t>(val);
            }
        }
    }
    return sum;
}