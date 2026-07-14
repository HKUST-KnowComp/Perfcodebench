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

            // Pruning: if the block's range is completely outside [low, high], skip it
            if (b_min > high || b_max < low) {
                continue;
            }

            // If the block is entirely within [low, high], we can sum everything without checks
            if (b_min >= low && b_max <= high) {
                const uint32_t* block_start = val_ptr + (b * block_size);
                size_t limit = (b + 1) * block_size;
                if (limit > num_values) limit = num_values;
                
                uint64_t block_sum = 0;
                size_t i = b * block_size;
                // Unroll for performance
                for (; i + 3 < limit; i += 4) {
                    block_sum += val_ptr[i];
                    block_sum += val_ptr[i+1];
                    block_sum += val_ptr[i+2];
                    block_sum += val_ptr[i+3];
                }
                for (; i < limit; ++i) {
                    block_sum += val_ptr[i];
                }
                current_sum += block_sum;
            } else {
                // Partial overlap: must check individual elements
                const uint32_t* block_start = val_ptr + (b * block_size);
                size_t limit = (b + 1) * block_size;
                if (limit > num_values) limit = num_values;

                for (size_t i = b * block_size; i < limit; ++i) {
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