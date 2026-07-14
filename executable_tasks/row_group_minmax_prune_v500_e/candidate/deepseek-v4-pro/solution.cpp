#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {

    // If metadata or block_size are invalid, fall back to full scan
    if (block_size == 0 || mins.empty() || maxs.empty()) {
        uint64_t sum = 0;
        for (int iter = 0; iter < iters; ++iter) {
            sum = 0;
            const uint32_t* d = values.data();
            size_t n = values.size();
            for (size_t i = 0; i < n; ++i) {
                uint32_t v = d[i];
                if (v >= low && v <= high) sum += v;
            }
        }
        return sum;
    }

    size_t num_blocks = values.size() / block_size;
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        const uint32_t* val_ptr = values.data();

        for (size_t b = 0; b < num_blocks; ++b) {
            // Check if the whole block is outside the range
            if (mins[b] > high || maxs[b] < low) {
                val_ptr += block_size;
                continue;
            }

            // Block may contain some values in range; scan it
            const uint32_t* block_end = val_ptr + block_size;
            while (val_ptr != block_end) {
                uint32_t v = *val_ptr;
                if (v >= low && v <= high) {
                    sum += v;
                }
                ++val_ptr;
            }
            // val_ptr now points to the start of the next block, no extra advance needed
        }
    }

    return sum;
}