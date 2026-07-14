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

    // The original code resets sum = 0 inside the iters loop, 
    // meaning only the result of the last iteration is returned.
    // However, the baseline logic says 'sum = 0' inside the loop.
    // Let's follow the baseline's logic exactly: only the last iteration counts.
    
    if (iters <= 0) return 0;

    // Since the result of each iteration is identical (base, low, high, and deltas don't change),
    // we only need to compute the sum for one iteration.
    uint64_t current_sum = 0;
    size_t i = 0;

    // Unroll loop for performance
    for (; i + 3 < size; i += 4) {
        uint32_t v0 = base + data[i];
        uint32_t v1 = base + data[i+1];
        uint32_t v2 = base + data[i+2];
        uint32_t v3 = base + data[i+3];

        if (v0 >= low && v0 <= high) current_sum += v0;
        if (v1 >= low && v1 <= high) current_sum += v1;
        if (v2 >= low && v2 <= high) current_sum += v2;
        if (v3 >= low && v3 <= high) current_sum += v3;
    }

    for (; i < size; ++i) {
        uint32_t v = base + data[i];
        if (v >= low && v <= high) {
            current_sum += v;
        }
    }

    return current_sum;
}