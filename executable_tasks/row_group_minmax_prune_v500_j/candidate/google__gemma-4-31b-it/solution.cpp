#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    
    const uint32_t* val_ptr = values.data();
    const uint32_t* min_ptr = mins.data();
    const uint32_t* max_ptr = maxs.data();
    const size_t num_values = values.size();
    const size_t num_blocks = mins.size();

    uint64_t final_sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            uint32_t b_min = min_ptr[b];
            uint32_t b_max = max_ptr[b];

            // Pruning: if the block's range is entirely outside [low, high], skip it
            if (b_min > high || b_max < low) {
                continue;
            }

            // If the block is entirely within [low, high], we can potentially optimize,
            // but for general cases, we scan the block.
            const uint32_t* block_start = val_ptr + (b * block_size);
            size_t end = (b + 1) * block_size;
            if (end > num_values) end = num_values;

            // Manual unrolling for the inner loop to improve throughput
            size_t i = b * block_size;
            while (i + 3 < end) {
                uint32_t v0 = val_ptr[i++];
                uint32_t v1 = val_ptr[i++];
                uint32_t v2 = val_ptr[i++];
                uint32_t v3 = val_ptr[i++];
                
                if (v0 >= low && v0 <= high) current_sum += v0;
                if (v1 >= low && v1 <= high) current_sum += v1;
                if (v2 >= low && v2 <= high) current_sum += v2;
                if (v3 >= low && v3 <= high) current_sum += v3;
            }
            while (i < end) {
                uint32_t v = val_ptr[i++];
                if (v >= low && v <= high) current_sum += v;
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}