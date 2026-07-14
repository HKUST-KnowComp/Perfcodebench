#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    // Early exit if no possible values can fall in the target range
    const uint32_t max_decoded = base + 0xFFFFU;
    if (max_decoded < low || high < base) {
        return 0;
    }

    // Precompute valid delta thresholds once to avoid per-iteration calculations
    const uint32_t min_delta = (low <= base) ? 0U : (low - base);
    const uint32_t max_delta = (high >= max_decoded) ? 0xFFFFU : (high - base);

    const uint16_t* delta_data = deltas.data();
    const size_t delta_count = deltas.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        // Single pass over input deltas: decode, filter, sum in one cache-efficient loop
        for (size_t i = 0; i < delta_count; ++i) {
            const uint16_t d = delta_data[i];
            if (d >= min_delta && d <= max_delta) {
                sum += static_cast<uint64_t>(base + d);
            }
        }
    }
    return sum;
}