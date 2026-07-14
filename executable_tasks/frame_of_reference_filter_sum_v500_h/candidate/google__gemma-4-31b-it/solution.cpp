#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    uint64_t total_sum = 0;
    const size_t n = deltas.size();
    const uint16_t* data = deltas.data();

    // Pre-calculate relative bounds to avoid adding 'base' inside the loop
    // value = base + delta
    // low <= base + delta <= high
    // low - base <= delta <= high - base
    
    // Handle underflow/overflow for relative bounds
    uint32_t rel_low = (low > base) ? (low - base) : 0;
    uint32_t rel_high = (high >= base) ? (high - base) : 0xFFFFFFFF;

    // If the range is completely outside the possible delta range [0, 65535]
    if (rel_low > 65535 || (high < base)) {
        return 0;
    }

    // Clamp rel_high to the maximum possible uint16_t value
    uint32_t clamped_high = (rel_high > 65535) ? 65535 : rel_high;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            uint32_t delta = data[i];
            if (delta >= rel_low && delta <= clamped_high) {
                current_sum += (static_cast<uint64_t>(base) + delta);
            }
        }
        total_sum = current_sum;
    }

    return total_sum;
}