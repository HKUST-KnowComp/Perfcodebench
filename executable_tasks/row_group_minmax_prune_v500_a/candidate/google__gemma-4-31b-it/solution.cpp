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
    
    if (values.empty()) return 0;

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

            // Pruning: if the block's range is completely outside [low, high]
            if (b_min > high || b_max < low) {
                continue;
            }

            const uint32_t* block_start = val_ptr + (b * block_size);
            
            // Optimization: if the block is entirely within [low, high], sum everything
            if (b_min >= low && b_max <= high) {
                uint64_t block_sum = 0;
                size_t count = block_size;
                // Handle potential trailing elements if block_size doesn't divide num_values perfectly
                if (b == num_blocks - 1) {
                    count = num_values - (b * block_size);
                }
                
                size_t i = 0;
                while (i + 3 < count) {
                    block_sum += block_start[i];
                    block_sum += block_start[i+1];
                    block_sum += block_start[i+2];
                    block_sum += block_start[i+3];
                    i += 4;
                }
                while (i < count) {
                    block_sum += block_start[i++];
                }
                current_sum += block_sum;
            } else {
                // Partial overlap: scan and filter
                size_t count = block_size;
                if (b == num_blocks - 1) {
                    count = num_values - (b * block_size);
                }
                
                for (size_t i = 0; i < count; ++i) {
                    uint32_t v = block_start[i];
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