#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters)
{
    const uint32_t n_blocks = mins.size();
    const uint32_t n_vals   = values.size();

    // Pre-compute per-block partial sums for the inclusive range [low,high]
    std::vector<uint64_t> block_sums(n_blocks, 0);
    for (uint32_t b = 0; b < n_blocks; ++b) {
        const uint32_t base = b * block_size;
        const uint32_t last = std::min(base + block_size, n_vals);
        uint64_t s = 0;
        for (uint32_t i = base; i < last; ++i) {
            uint32_t v = values[i];
            // branch-free selection
            s += (v >= low && v <= high) ? static_cast<uint64_t>(v) : 0;
        }
        block_sums[b] = s;
    }

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_sum = 0;
        // skip blocks that cannot contribute
        for (uint32_t b = 0; b < n_blocks; ++b) {
            if (maxs[b] >= low && mins[b] <= high) {
                iter_sum += block_sums[b];
            }
        }
        total = iter_sum;
    }
    return total;
}