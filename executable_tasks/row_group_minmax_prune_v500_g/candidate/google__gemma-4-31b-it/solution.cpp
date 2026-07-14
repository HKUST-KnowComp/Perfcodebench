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
    
    uint64_t final_sum = 0;
    const uint32_t* val_ptr = values.data();
    const uint32_t* min_ptr = mins.data();
    const uint32_t* max_ptr = maxs.data();
    const size_t num_values = values.size();
    const size_t num_groups = mins.size();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (size_t g = 0; g < num_groups; ++g) {
            // Pruning: if the group's range is completely outside [low, high], skip it
            if (max_ptr[g] < low || min_ptr[g] > high) {
                continue;
            }

            // If the group's range is entirely within [low, high], we could potentially
            // sum all elements without checking, but we don't have precomputed group sums.
            // We must iterate through the block.
            const uint32_t* block_start = val_ptr + (g * block_size);
            const uint32_t* block_end = block_start + block_size;
            
            // Ensure we don't go out of bounds of the values vector
            if (block_end > val_ptr + num_values) {
                block_end = val_ptr + num_values;
            }

            #pragma omp simd reduction(+:current_sum)
            for (const uint32_t* p = block_start; p < block_end; ++p) {
                uint32_t v = *p;
                if (v >= low && v <= high) {
                    current_sum += v;
                }
            }
        }
        final_sum = current_sum;
    }
    return final_sum;
}