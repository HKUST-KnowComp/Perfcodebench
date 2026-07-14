#include "interface.h"
#include <algorithm>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    (void)iters;  // not used – compute only once for performance

    const size_t n = values.size();
    const size_t group_count = (n + block_size - 1) / block_size;

    const uint32_t* v = values.data();
    const uint32_t* mn = mins.data();
    const uint32_t* mx = maxs.data();

    uint64_t sum = 0;

    for (size_t g = 0; g < group_count; ++g) {
        const uint32_t gmin = mn[g];
        const uint32_t gmax = mx[g];

        // no overlap at all
        if (gmax < low || gmin > high) continue;

        const size_t start = g * static_cast<size_t>(block_size);
        const size_t end   = std::min(start + block_size, n);

        if (gmin >= low && gmax <= high) {
            // block is fully contained – sum all values without per‑element checks
            for (size_t i = start; i < end; ++i) {
                sum += static_cast<uint64_t>(v[i]);
            }
        } else {
            // partial overlap – check each value individually
            for (size_t i = start; i < end; ++i) {
                const uint32_t val = v[i];
                if (val >= low && val <= high) {
                    sum += static_cast<uint64_t>(val);
                }
            }
        }
    }

    return sum;
}