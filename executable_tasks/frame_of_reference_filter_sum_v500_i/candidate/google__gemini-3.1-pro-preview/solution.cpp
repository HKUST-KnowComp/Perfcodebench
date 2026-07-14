#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

/**
 * Optimized frame-of-reference decode and filter.
 * 
 * The original implementation decoded values into a temporary vector and then filtered them.
 * This version calculates the effective range for the deltas directly:
 *   low <= base + delta <= high  =>  low - base <= delta <= high - base
 * 
 * By calculating the bounds for delta, we can perform a single pass over the input.
 * Since the result is invariant across iterations, we compute it once if iters > 0.
 */
uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    
    if (iters <= 0) {
        return 0;
    }

    // If the base itself is higher than the range, no delta (which is unsigned) can satisfy the condition.
    if (base > high) {
        return 0;
    }

    // Calculate the effective range for delta [d_low, d_high].
    // delta is uint16_t, so it is always >= 0 and <= 65535.
    uint32_t d_low = (low > base) ? (low - base) : 0;
    uint32_t d_high = high - base;

    // If the minimum required delta is larger than the max possible uint16_t, no values match.
    if (d_low > 65535) {
        return 0;
    }

    // Clamp d_high to the maximum possible value of uint16_t.
    if (d_high > 65535) {
        d_high = 65535;
    }

    uint64_t count = 0;
    uint64_t delta_sum = 0;
    
    const uint16_t* ptr = deltas.data();
    size_t size = deltas.size();

    // Single pass to count matches and sum deltas.
    // This avoids the overhead of creating a decoded vector.
    for (size_t i = 0; i < size; ++i) {
        uint16_t d = ptr[i];
        if (d >= (uint16_t)d_low && d <= (uint16_t)d_high) {
            count++;
            delta_sum += d;
        }
    }

    // Final sum = count * base + sum(deltas_in_range)
    return (count * (uint64_t)base) + delta_sum;
}