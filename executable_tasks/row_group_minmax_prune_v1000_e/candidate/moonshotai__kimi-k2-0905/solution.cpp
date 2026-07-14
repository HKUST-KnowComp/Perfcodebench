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
    const uint32_t n_vals   = values.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        uint32_t base = 0;
        for (uint32_t b = 0; b < n_blocks; ++b) {
            const uint32_t mn = mins[b];
            const uint32_t mx = maxs[b];
            if (mn > high || mx < low) {
                base += block_size;
                continue;
            }
            const uint32_t block_end = (b == n_blocks - 1) ? n_vals : base + block_size;
            const uint32_t* __restrict ptr = values.data() + base;
            const uint32_t* const end = values.data() + block_end;

            uint64_t local = 0;
            const uint32_t l = low;
            const uint32_t h = high;

            uint32_t i = 0;
            const uint32_t simd = (block_end - base) & ~3U;
            for (; i < simd; i += 4) {
                uint32_t v0 = ptr[i+0];
                uint32_t v1 = ptr[i+1];
                uint32_t v2 = ptr[i+2];
                uint32_t v3 = ptr[i+3];
                local += (v0 >= l && v0 <= h) ? static_cast<uint64_t>(v0) : 0;
                local += (v1 >= l && v1 <= h) ? static_cast<uint64_t>(v1) : 0;
                local += (v2 >= l && v2 <= h) ? static_cast<uint64_t>(v2) : 0;
                local += (v3 >= l && v3 <= h) ? static_cast<uint64_t>(v3) : 0;
            }
            for (; i < block_end - base; ++i) {
                uint32_t v = ptr[i];
                if (v >= l && v <= h) local += static_cast<uint64_t>(v);
            }
            sum += local;
            base = block_end;
        }
    }
    return sum;
}