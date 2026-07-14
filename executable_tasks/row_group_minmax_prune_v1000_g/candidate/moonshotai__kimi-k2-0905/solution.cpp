#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters)
{
    const size_t n_blocks = mins.size();
    // Pre-compute inclusive sums for each block (done once, outside iter-loop)
    std::vector<uint64_t> inclusive(n_blocks, 0);
    for (size_t b = 0; b < n_blocks; ++b) {
        uint64_t block_sum = 0;
        size_t base = b * block_size;
        size_t limit = base + block_size;
        if (limit > values.size()) limit = values.size();
        for (size_t i = base; i < limit; ++i) {
            block_sum += static_cast<uint64_t>(values[i]);
        }
        inclusive[b] = block_sum;
    }

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        total = 0;
        for (size_t b = 0; b < n_blocks; ++b) {
            uint32_t bmin = mins[b];
            uint32_t bmax = maxs[b];
            if (bmax < low || bmin > high) {
                // Block outside range => skip
                continue;
            }
            if (bmin >= low && bmax <= high) {
                // Block fully inside => use pre-computed inclusive sum
                total += inclusive[b];
            } else {
                // Straddles boundary => scan the block
                size_t base = b * block_size;
                size_t limit = base + block_size;
                if (limit > values.size()) limit = values.size();
                for (size_t i = base; i < limit; ++i) {
                    uint32_t v = values[i];
                    if (v >= low && v <= high) {
                        total += static_cast<uint64_t>(v);
                    }
                }
            }
        }
    }
    return total;
}