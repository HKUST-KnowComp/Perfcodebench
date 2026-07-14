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
    int iters) {
    (void)iters;
    size_t n = values.size();
    if (n == 0) return 0;
    size_t n_blocks = (n + block_size - 1) / block_size;
    uint64_t total_sum = 0;
    for (size_t b = 0; b < n_blocks; ++b) {
        size_t start = b * block_size;
        size_t end = std::min(start + block_size, n);
        if (mins[b] > high || maxs[b] < low) continue;
        if (mins[b] >= low && maxs[b] <= high) {
            for (size_t i = start; i < end; ++i) total_sum += values[i];
        } else {
            for (size_t i = start; i < end; ++i) {
                uint32_t v = values[i];
                if (v >= low && v <= high) total_sum += v;
            }
        }
    }
    return total_sum;
}
