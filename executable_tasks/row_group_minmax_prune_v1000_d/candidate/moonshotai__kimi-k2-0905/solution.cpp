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
    (void)block_size;  // not needed; we use mins/maxs lengths
    const size_t num_groups = mins.size();
    const uint32_t* val = values.data();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        total = 0;
        size_t offset = 0;
        for (size_t g = 0; g < num_groups; ++g) {
            uint32_t g_min = mins[g];
            uint32_t g_max = maxs[g];
            if (g_max < low || g_min > high) {
                offset += block_size;
                continue;
            }
            const uint32_t* group_start = val + offset;
            const uint32_t* group_end   = group_start + block_size;
            uint64_t local = 0;
            for (const uint32_t* p = group_start; p != group_end; ++p) {
                uint32_t v = *p;
                local += (v >= low && v <= high) ? static_cast<uint64_t>(v) : 0;
            }
            total += local;
            offset += block_size;
        }
    }
    return total;
}