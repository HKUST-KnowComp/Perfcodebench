#include "interface.h"
#include <cstddef>
#include <cstdint>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters)
{
    (void)iters;                        // we only need to scan once
    const size_t blocks = mins.size();
    uint64_t sum = 0;

    const uint32_t*  v = values.data();
    const uint32_t*  mn = mins.data();
    const uint32_t*  mx = maxs.data();

    for (size_t b = 0; b < blocks; ++b) {
        const uint32_t block_min = mn[b];
        const uint32_t block_max = mx[b];

        if (block_min > high || block_max < low)      // whole block out of range
            continue;

        const size_t start = b * block_size;
        const size_t limit = start + block_size;

        size_t i = start;
        const size_t vec_end = start + ((block_size >> 2) << 2);

        // unroll 4× to expose vectorisation
        for (; i < vec_end; i += 4) {
            uint32_t v0 = v[i + 0];
            uint32_t v1 = v[i + 1];
            uint32_t v2 = v[i + 2];
            uint32_t v3 = v[i + 3];

            sum += (v0 >= low & v0 <= high) ? v0 : 0;
            sum += (v1 >= low & v1 <= high) ? v1 : 0;
            sum += (v2 >= low & v2 <= high) ? v2 : 0;
            sum += (v3 >= low & v3 <= high) ? v3 : 0;
        }
        // tail
        for (; i < limit; ++i) {
            uint32_t val = v[i];
            if (val >= low && val <= high)
                sum += val;
        }
    }
    return sum;
}