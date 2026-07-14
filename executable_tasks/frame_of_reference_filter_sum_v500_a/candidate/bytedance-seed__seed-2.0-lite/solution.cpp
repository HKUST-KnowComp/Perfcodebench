#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    // Precompute valid delta range once to avoid repeated calculations
    const uint32_t delta_low = (low > base) ? (low - base) : 0U;
    uint32_t delta_high = (high < base) ? 0U : (high - base);
    // Cap delta to max uint16_t since all stored deltas fit in this type
    if (delta_high > UINT16_MAX) delta_high = UINT16_MAX;

    uint64_t sum = 0;
    const uint16_t* const delta_data = deltas.data();
    const std::size_t delta_count = deltas.size();

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        // Single pass decode + sum to minimize memory traffic
        for (std::size_t i = 0; i < delta_count; ++i) {
            const uint16_t d = delta_data[i];
            if (d >= delta_low && d <= delta_high) {
                sum += static_cast<uint64_t>(base + d);
            }
        }
    }
    return sum;
}