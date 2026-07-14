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
    const size_t n = values.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        size_t block_idx = 0;
        size_t pos = 0;

        while (pos < n) {
            const uint32_t blk_min = mins[block_idx];
            const uint32_t blk_max = maxs[block_idx];
            const size_t next = (pos + block_size < n) ? pos + block_size : n;
            const size_t len = next - pos;

            if (blk_max >= low && blk_min <= high) {
                const uint32_t* ptr = v + pos;
                const uint32_t* end = ptr + (len & ~size_t(3));

                while (ptr != end) {
                    uint32_t a = ptr[0];
                    uint32_t b = ptr[1];
                    uint32_t c = ptr[2];
                    uint32_t d = ptr[3];
                    uint64_t m0 = (a >= low) & (a <= high);
                    uint64_t m1 = (b >= low) & (b <= high);
                    uint64_t m2 = (c >= low) & (c <= high);
                    uint64_t m3 = (d >= low) & (d <= high);
                    sum += m0 * a;
                    sum += m1 * b;
                    sum += m2 * c;
                    sum += m3 * d;
                    ptr += 4;
                }

                const uint32_t* tail = v + next;
                while (ptr != tail) {
                    uint32_t x = *ptr++;
                    if (x >= low && x <= high) sum += x;
                }
            }

            pos = next;
            ++block_idx;
        }

        total = sum;
    }

    return total;
}