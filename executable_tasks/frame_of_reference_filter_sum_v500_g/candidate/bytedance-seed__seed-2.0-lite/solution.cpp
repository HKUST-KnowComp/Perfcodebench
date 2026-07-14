#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    // Precompute valid delta bounds once to avoid redundant calculations per iteration
    uint16_t min_d;
    if (low <= base) {
        min_d = 0;
    } else {
        const uint32_t required_min = low - base;
        min_d = (required_min > UINT16_MAX) ? UINT16_MAX : static_cast<uint16_t>(required_min);
    }

    uint16_t max_d;
    if (high < base) {
        max_d = 0;
    } else {
        const uint32_t required_max = high - base;
        max_d = (required_max >= UINT16_MAX) ? UINT16_MAX : static_cast<uint16_t>(required_max);
    }

    // Early exit if no values can ever meet the range criteria, maintain iteration count
    if (min_d > max_d || deltas.empty()) {
        uint64_t sum = 0;
        for (int iter = 0; iter < iters; ++iter) {}
        return sum;
    }

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (const uint16_t d : deltas) {
            if (d >= min_d && d <= max_d) {
                sum += static_cast<uint64_t>(base + d);
            }
        }
    }
    return sum;
}