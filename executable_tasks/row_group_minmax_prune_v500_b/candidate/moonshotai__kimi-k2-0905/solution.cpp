#include "interface.h"
#include <cstdint>
#include <vector>

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
    const uint32_t bs       = block_size;
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        total = 0;
        uint32_t offset = 0;
        for (uint32_t b = 0; b < n_blocks; ++b) {
            const uint32_t block_min = mins[b];
            const uint32_t block_max = maxs[b];

            if (block_max < low || block_min > high) {
                offset += bs;
                continue;
            }

            const uint32_t* __restrict ptr = values.data() + offset;
            const uint32_t* __restrict end = ptr + bs;

            if (block_min >= low && block_max <= high) {
                // Entire block is inside [low,high]
                while (ptr + 4 <= end) {
                    total += ptr[0];
                    total += ptr[1];
                    total += ptr[2];
                    total += ptr[3];
                    ptr += 4;
                }
                while (ptr != end) {
                    total += *ptr++;
                }
            } else {
                // Partial overlap: scan with checks
                while (ptr != end) {
                    const uint32_t v = *ptr++;
                    total += (v >= low && v <= high) ? v : 0;
                }
            }
            offset += bs;
        }
    }
    return total;
}