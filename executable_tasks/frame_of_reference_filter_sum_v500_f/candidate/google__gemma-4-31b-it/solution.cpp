#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    uint64_t total_sum = 0;
    const uint16_t* data = deltas.data();
    const size_t size = deltas.size();

    // Pre-calculate range relative to base to avoid repeated additions inside the loop
    // value = base + delta
    // low <= base + delta <= high  <=>  low - base <= delta <= high - base
    // Since delta is uint16_t, we can handle the bounds carefully.
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t i = 0; i < size; ++i) {
            uint32_t val = base + data[i];
            if (val >= low && val <= high) {
                current_sum += val;
            }
        }
        total_sum = current_sum;
    }
    
    // The original code resets sum = 0 inside the loop and returns the sum of the LAST iteration.
    // Based on the baseline: sum = 0; for(...) { sum += ... } is inside the iters loop.
    // The baseline returns the sum of the final iteration only.
    return total_sum;
}