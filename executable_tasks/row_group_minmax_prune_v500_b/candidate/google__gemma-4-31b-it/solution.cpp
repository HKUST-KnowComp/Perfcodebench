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

            // If the block is entirely within [low, high], we can sum everything without checks
            // However, for simplicity and to avoid complex logic, we check if it's partially overlapping
            const uint32_t* block_start = val_ptr + (b * block_size);
            const uint32_t* block_end = block_start + block_size;

            if (b_min >= low && b_max <= high) {
                // Fast path: all elements in block are within range
                uint64_t block_sum = 0;
                size_t i = 0;
                // Unroll loop for performance
                for (; i + 3 < block_size; i += 4) {
                    block_sum += block_start[i];
                    block_sum += block_start[i+1];
                    block_sum += block_start[i+2];
                    block_sum += block_start[i+3];
                }
                for (; i < block_size; ++i) {
                    block_sum += block_start[i];
                }
                current_sum += block_sum;
            } else {
                // Slow path: check individual elements
                for (size_t i = 0; i < block_size; ++i) {
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