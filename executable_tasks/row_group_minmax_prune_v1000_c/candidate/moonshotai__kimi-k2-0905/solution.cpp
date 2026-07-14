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
    const uint32_t* v = values.data();
    const uint32_t* mn = mins.data();
    const uint32_t* mx = maxs.data();
    const size_t num_blocks = mins.size();
    const size_t total = values.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t base = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            const uint32_t block_min = mn[b];
            const uint32_t block_max = mx[b];
            // unsigned overlap test: [block_min,block_max] intersects [low,high]
            if (block_min <= high && block_max >= low) {
                const size_t end = (base + block_size < total) ? base + block_size : total;
                // fully contained fast path
                if (block_min >= low && block_max <= high) {
                    for (size_t i = base; i < end; ++i) {
                        sum += static_cast<uint64_t>(v[i]);
                    }
                } else {
                    // partial overlap: check each element
                    const uint32_t l = low;
                    const uint32_t h = high;
                    size_t i = base;
                    // unroll 8x
                    for (; i + 8 <= end; i += 8) {
                        uint64_t s0 = (v[i+0] >= l && v[i+0] <= h) ? v[i+0] : 0;
                        uint64_t s1 = (v[i+1] >= l && v[i+1] <= h) ? v[i+1] : 0;
                        uint64_t s2 = (v[i+2] >= l && v[i+2] <= h) ? v[i+2] : 0;
                        uint64_t s3 = (v[i+3] >= l && v[i+3] <= h) ? v[i+3] : 0;
                        uint64_t s4 = (v[i+4] >= l && v[i+4] <= h) ? v[i+4] : 0;
                        uint64_t s5 = (v[i+5] >= l && v[i+5] <= h) ? v[i+5] : 0;
                        uint64_t s6 = (v[i+6] >= l && v[i+6] <= h) ? v[i+6] : 0;
                        uint64_t s7 = (v[i+7] >= l && v[i+7] <= h) ? v[i+7] : 0;
                        sum += s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7;
                    }
                    for (; i < end; ++i) {
                        uint32_t val = v[i];
                        if (val >= l && val <= h) sum += val;
                    }
                }
            }
            base += block_size;
            if (base >= total) break;
        }
    }
    return sum;
}