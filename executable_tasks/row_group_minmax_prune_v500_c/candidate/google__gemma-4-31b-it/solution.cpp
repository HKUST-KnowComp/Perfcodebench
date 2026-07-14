#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    
    uint64_t final_sum = 0;
    const uint32_t* val_ptr = values.data();
    const uint32_t* min_ptr = mins.data();
    const uint32_t* max_ptr = maxs.data();
    const size_t num_values = values.size();
    const size_t num_groups = mins.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t g = 0; g < num_groups; ++g) {
            uint32_t g_min = min_ptr[g];
            uint32_t g_max = max_ptr[g];

            // Pruning: if the group's range is completely outside [low, high], skip it
            if (g_min > high || g_max < low) {
                continue;
            }

            // If the group's range is entirely within [low, high], we can sum all elements
            // However, for simplicity and to avoid branching inside the inner loop,
            // we check the range. Most compilers will vectorize the following loop.
            const uint32_t* block_start = val_ptr + (g * block_size);
            const uint32_t* block_end = block_start + block_size;
            
            // Safety check for the last block if block_size doesn't perfectly divide num_values
            if (block_end > val_ptr + num_values) {
                block_end = val_ptr + num_values;
            }

            if (g_min >= low && g_max <= high) {
                // Fast path: all elements in this block are guaranteed to be in range
                uint64_t block_sum = 0;
                for (const uint32_t* p = block_start; p < block_end; ++p) {
                    block_sum += *p;
                }
                current_sum += block_sum;
            } else {
                // Slow path: filter elements
                uint64_t block_sum = 0;
                for (const uint32_t* p = block_start; p < block_end; ++p) {
                    uint32_t v = *p;
                    if (v >= low && v <= high) {
                        block_sum += v;
                    }
                }
                current_sum += block_sum;
            }
        }
        final_sum = current_sum;
    }

    return final_sum;
}