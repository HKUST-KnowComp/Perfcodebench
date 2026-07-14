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

    // Pre-calculate which blocks are relevant to avoid repeating the check in the iters loop
    // However, since the goal is to optimize the total time and the blocks are small, 
    // we can just iterate through the blocks inside the loop.
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            // Pruning: if the block's range [min, max] does not overlap with [low, high]
            if (min_ptr[b] > high || max_ptr[b] < low) {
                continue;
            }
            
            // If the block is entirely within [low, high], we can sum everything without checks
            if (min_ptr[b] >= low && max_ptr[b] <= high) {
                const uint32_t* block_start = val_ptr + (b * block_size);
                for (uint32_t i = 0; i < block_size; ++i) {
                    current_sum += block_start[i];
                }
            } else {
                // Partial overlap: check individual elements
                const uint32_t* block_start = val_ptr + (b * block_size);
                for (uint32_t i = 0; i < block_size; ++i) {
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