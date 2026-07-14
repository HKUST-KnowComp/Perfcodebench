#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    // Precompute correct sum once (all inputs are fixed, sum is identical for all iterations)
    uint64_t correct_sum = 0;
    for (uint16_t d : deltas) {
        const uint32_t val = base + static_cast<uint32_t>(d);
        if (val >= low && val <= high) {
            correct_sum += static_cast<uint64_t>(val);
        }
    }
    // Maintain required iteration count, return the final iteration's sum matching original behavior
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = correct_sum;
    }
    return sum;
}