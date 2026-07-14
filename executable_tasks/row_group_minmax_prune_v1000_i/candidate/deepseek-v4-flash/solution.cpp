#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {

    const size_t n = values.size();
    const size_t num_blocks = (n + block_size - 1) / block_size;
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            const uint32_t block_min = mins[b];
            const uint32_t block_max = maxs[b];
            // skip block entirely if it doesn't intersect the range
            if (block_max < low || block_min > high) {
                continue;
            }
            const size_t start = b * static_cast<size_t>(block_size);
            size_t end = start + block_size;
            if (end > n) end = n;

            const uint32_t* vptr = values.data() + start;
            const size_t count = end - start;

            if (block_min >= low && block_max <= high) {
                // entire block is within range -> sum unconditionally
                for (size_t i = 0; i < count; ++i) {
                    sum += static_cast<uint64_t>(vptr[i]);
                }
            } else {
                // partial overlap -> need to check each value
                for (size_t i = 0; i < count; ++i) {
                    const uint32_t val = vptr[i];
                    if (val >= low && val <= high) {
                        sum += static_cast<uint64_t>(val);
                    }
                }
            }
        }
    }
    return sum;
}
