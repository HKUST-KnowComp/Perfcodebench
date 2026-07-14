#include "interface.h"
#include <algorithm>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    
    uint64_t total_sum = 0;
    const uint32_t* val_ptr = values.data();
    const uint32_t* min_ptr = mins.data();
    const uint32_t* max_ptr = maxs.data();
    const size_t num_values = values.size();
    const size_t num_blocks = mins.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t b = 0; b < num_blocks; ++b) {
            // Pruning: if the block's range is completely outside [low, high], skip it
            if (max_ptr[b] < low || min_ptr[b] > high) {
                val_ptr += block_size;
                continue;
            }
            
            // If the block is entirely within [low, high], we could potentially sum all,
            // but the task requires checking each value for the range filter correctness
            // unless we have a precomputed block sum. Since we don't, we scan.
            const uint32_t* block_start = val_ptr;
            const uint32_t* block_end = val_ptr + block_size;
            
            // Bounds check for the last block if block_size is not a perfect divisor
            if (block_end > values.data() + num_values) {
                block_end = values.data() + num_values;
            }

            for (const uint32_t* p = block_start; p < block_end; ++p) {
                uint32_t v = *p;
                if (v >= low && v <= high) {
                    current_sum += v;
                }
            }
            val_ptr += block_size;
        }
        total_sum = current_sum;
        // Reset pointer for next iteration
        val_ptr = values.data();
    }
    
    return total_sum;
}