#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    
    if (values.empty()) return 0;

    const uint32_t* val_ptr = values.data();
    const uint32_t* min_ptr = mins.data();
    const uint32_t* max_ptr = maxs.data();
    size_t num_values = values.size();
    size_t num_blocks = mins.size();

    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            uint32_t b_min = min_ptr[b];
            uint32_t b_max = max_ptr[b];

            // Pruning: if the block's range is completely outside [low, high], skip it
            if (b_min > high || b_max < low) {
                continue;
            }

            // If the block is entirely within [low, high], we can sum everything without checks
            // However, for simplicity and to avoid potential overflow/logic errors in complex cases,
            // we check the range. But we can optimize the inner loop.
            const uint32_t* block_start = val_ptr + (b * block_size);
            size_t end_offset = (b + 1) * block_size;
            if (end_offset > num_values) end_offset = num_values;

            if (b_min >= low && b_max <= high) {
                for (size_t i = b * block_size; i < end_offset; ++i) {
                    current_sum += val_ptr[i];
                }
            } else {
                for (size_t i = b * block_size; i < end_offset; ++i) {
                    uint32_t v = val_ptr[i];
                    if (v >= low && v <= high) {
                        current_sum += v;
                    }
                }
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}