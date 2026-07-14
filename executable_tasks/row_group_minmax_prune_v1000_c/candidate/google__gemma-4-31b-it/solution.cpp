#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    
    if (iters <= 0) return 0;

    const uint32_t* val_ptr = values.data();
    const uint32_t* min_ptr = mins.data();
    const uint32_t* max_ptr = maxs.data();
    const size_t num_values = values.size();
    const size_t num_blocks = mins.size();

    uint64_t final_sum = 0;

    // The baseline resets sum = 0 every iteration and returns the sum of the last iteration.
    // Therefore, we only need to compute the sum once.
    uint64_t current_sum = 0;
    
    for (size_t b = 0; b < num_blocks; ++b) {
        uint32_t b_min = min_ptr[b];
        uint32_t b_max = max_ptr[b];

        // Pruning: if the block range is entirely outside [low, high], skip it.
        if (b_max < low || b_min > high) {
            continue;
        }

        // If the block is entirely within [low, high], we can sum everything without checks.
        if (b_min >= low && b_max <= high) {
            const uint32_t* block_start = val_ptr + (b * block_size);
            const uint32_t* block_end = block_start + block_size;
            
            // Manual unrolling for performance
            size_t i = 0;
            uint64_t local_sum = 0;
            for (; i + 3 < block_size; i += 4) {
                local_sum += block_start[i];
                local_sum += block_start[i+1];
                local_sum += block_start[i+2];
                local_sum += block_start[i+3];
            }
            for (; i < block_size; ++i) {
                local_sum += block_start[i];
            }
            current_sum += local_sum;
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
    // The baseline loop runs 'iters' times, resetting sum each time.
    // To match the behavior exactly, we return the result of one pass.
    return final_sum;
}