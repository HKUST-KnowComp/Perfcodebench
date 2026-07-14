#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {

    const uint32_t* vptr = values.data();
    const uint32_t* minptr = mins.data();
    const uint32_t* maxptr = maxs.data();
    const size_t n = values.size();
    const size_t n_blocks = (n + block_size - 1) / block_size;
    const uint32_t range_len = high - low;

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t b = 0; b < n_blocks; ++b) {
            uint32_t bmin = minptr[b];
            uint32_t bmax = maxptr[b];
            if (bmax < low || bmin > high) continue;

            size_t start = b * block_size;
            size_t end = start + block_size;
            if (end > n) end = n;
            const uint32_t* ptr = vptr + start;
            const uint32_t* end_ptr = vptr + end;
            while (ptr < end_ptr) {
                uint32_t v = *ptr++;
                if ((v - low) <= range_len) {
                    sum += v;
                }
            }
        }
    }
    return sum;
}