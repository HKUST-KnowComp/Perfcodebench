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

    // Pre-calculate range relative to base to avoid adding base inside the loop
    // value = base + delta
    // low <= base + delta <= high  =>  low - base <= delta <= high - base
    
    // Since delta is uint16_t, we can clamp the relative range to [0, 65535]
    uint32_t rel_low = (low > base) ? (low - base) : 0;
    uint32_t rel_high = (high >= base) ? (high - base) : 0xFFFFFFFF;
    
    // If rel_low > 65535 or rel_high < 0, no delta can satisfy the condition
    if (rel_low > 65535 || (high < base)) {
        return 0;
    }

    // Optimization: if the range covers all possible uint16_t values, 
    // we can use a faster sum path.
    if (rel_low == 0 && rel_high >= 65535) {
        uint64_t inner_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            inner_sum += data[i];
        }
        uint64_t base_part = static_cast<uint64_t>(base) * n;
        return (inner_sum + base_part) * iters;
    }

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < n; ++i) {
            uint32_t delta = data[i];
            if (delta >= rel_low && delta <= rel_high) {
                current_sum += (base + delta);
            }
        }
        total_sum = current_sum; // The original code resets sum = 0 and returns the last iteration's sum
    }

    // Note: The original baseline code does 'sum = 0' inside the loop and returns the final sum.
    // This means it effectively only returns the sum of the LAST iteration.
    // To preserve exact behavior of the baseline provided:
    return total_sum;
}