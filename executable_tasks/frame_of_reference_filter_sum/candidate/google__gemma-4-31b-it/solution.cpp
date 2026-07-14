#include "interface.h"

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
    uint64_t total_sum = 0;
    
    // Pre-calculate bounds relative to base to avoid repeated additions inside the loop
    // We use uint64_t to avoid underflow/overflow during comparison
    uint64_t u_base = base;
    uint64_t u_low = low;
    uint64_t u_high = high;

    // The original code resets sum = 0 inside the loop, meaning only the last iteration counts.
    // However, the baseline logic says 'sum = 0' inside the loop, but the return is outside.
    // Looking at the baseline: 
    // for (int iter = 0; iter < iters; ++iter) { 
    //    ... 
    //    sum = 0; 
    //    for (uint32_t value : decoded) { ... sum += value; } 
    // }
    // This means the result is simply the sum of the final iteration.
    
    if (iters <= 0) return 0;

    const uint16_t* data = deltas.data();
    size_t size = deltas.size();
    uint64_t current_sum = 0;

    // Only the last iteration matters based on the baseline's 'sum = 0' placement
    // But to be safe and follow the logic exactly:
    for (int iter = 0; iter < iters; ++iter) {
        current_sum = 0;
        for (size_t i = 0; i < size; ++i) {
            uint32_t val = static_cast<uint32_t>(u_base + data[i]);
            if (val >= low && val <= high) {
                current_sum += val;
            }
        }
    }

    return current_sum;
}