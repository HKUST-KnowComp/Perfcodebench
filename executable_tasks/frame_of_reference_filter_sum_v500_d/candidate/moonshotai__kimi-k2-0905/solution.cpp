#include "interface.h"
#include <cstdint>
#include <cstddef>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters)
{
    // Pre-compute the inclusive 16-bit delta range that survives the filter.
    // Because base + delta must be in [low,high], delta must be in:
    //   [low - base, high - base]  (clamped to 0..65535)
    const uint32_t delta_low  = (low  > base) ? static_cast<uint16_t>(low  - base) : 0u;
    const uint32_t delta_high = (high < base + 0xFFFFu) ? static_cast<uint16_t>(high - base) : 0xFFFFu;

    const uint16_t* const data = deltas.data();
    const size_t n = deltas.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        size_t i = 0;

        // Process 8 deltas per iteration (unrolled)
        for (; i + 8 <= n; i += 8) {
            uint16_t d0 = data[i+0];
            uint16_t d1 = data[i+1];
            uint16_t d2 = data[i+2];
            uint16_t d3 = data[i+3];
            uint16_t d4 = data[i+4];
            uint16_t d5 = data[i+5];
            uint16_t d6 = data[i+6];
            uint16_t d7 = data[i+7];

            // Branchless conditional add: (d >= delta_low) & (d <= delta_high)
            auto acc = [&](uint16_t d) {
                uint32_t in_range = (d >= delta_low) & (d <= delta_high);
                sum += in_range * (base + static_cast<uint64_t>(d));
            };
            acc(d0); acc(d1); acc(d2); acc(d3);
            acc(d4); acc(d5); acc(d6); acc(d7);
        }
        // Tail
        for (; i < n; ++i) {
            uint16_t d = data[i];
            if (d >= delta_low && d <= delta_high) {
                sum += base + static_cast<uint64_t>(d);
            }
        }
        total = sum;
    }
    return total;
}