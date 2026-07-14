#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters)
{
    size_t size = values.size();
    size_t num_blocks = size / block_size;
    size_t tail_start = num_blocks * block_size;

    uint64_t total_sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        const uint32_t* val_ptr = values.data();

        // Process complete blocks
        for (size_t b = 0; b < num_blocks; ++b) {
            uint32_t bmin = mins[b];
            uint32_t bmax = maxs[b];

            // Entire block outside range
            if (bmax < low || bmin > high) {
                val_ptr += block_size;
                continue;
            }

            // Entire block inside range – sum unconditionally
            if (bmin >= low && bmax <= high) {
                const uint32_t* block_end = val_ptr + block_size;
                while (val_ptr < block_end) {
                    sum += *val_ptr;
                    ++val_ptr;
                }
            } else {
                // Partial overlap – check each value
                const uint32_t* block_end = val_ptr + block_size;
                while (val_ptr < block_end) {
                    uint32_t v = *val_ptr;
                    if (v >= low && v <= high) sum += v;
                    ++val_ptr;
                }
            }
        }

        // Process tail (remainder) without metadata
        for (size_t i = tail_start; i < size; ++i) {
            uint32_t v = values[i];
            if (v >= low && v <= high) sum += v;
        }

        total_sum = sum;
    }

    return total_sum;
}