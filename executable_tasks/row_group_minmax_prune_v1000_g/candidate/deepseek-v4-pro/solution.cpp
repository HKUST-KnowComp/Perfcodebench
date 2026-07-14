#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    size_t n = values.size();
    if (n == 0 || iters <= 0) return 0;
    size_t num_blocks = (n + block_size - 1) / block_size;
    uint64_t total_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            size_t start = b * block_size;
            size_t end = (start + block_size < n) ? start + block_size : n;
            uint32_t b_min = mins[b];
            uint32_t b_max = maxs[b];
            if (b_max < low || b_min > high) {
                continue;
            }
            if (b_min >= low && b_max <= high) {
                // fully contained block: no per-value check
                for (size_t i = start; i < end; ++i) {
                    sum += values[i];
                }
            } else {
                // partial overlap: check each value
                for (size_t i = start; i < end; ++i) {
                    uint32_t v = values[i];
                    if (v >= low && v <= high) {
                        sum += v;
                    }
                }
            }
        }
        total_sum = sum;
    }
    return total_sum;
}