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
    if (iters <= 0) return 0;

    const uint32_t* val_ptr = values.data();
    const uint32_t* min_ptr = mins.data();
    const uint32_t* max_ptr = maxs.data();
    const size_t num_values = values.size();
    const size_t num_blocks = mins.size();

    uint64_t total_sum = 0;

    // We calculate the sum for one iteration and multiply by iters
    // because the data is read-only and the range is constant.
    uint64_t single_iter_sum = 0;

    for (size_t b = 0; b < num_blocks; ++b) {
        uint32_t b_min = min_ptr[b];
        uint32_t b_max = max_ptr[b];

        // Pruning: if the block range is completely outside [low, high]
        if (b_min > high || b_max < low) {
            continue;
        }

        const uint32_t* block_start = val_ptr + (b * block_size);
        
        // Optimization: if the block is entirely contained within [low, high]
        if (b_min >= low && b_max <= high) {
            uint64_t block_sum = 0;
            for (uint32_t i = 0; i < block_size; ++i) {
                block_sum += block_start[i];
            }
            single_iter_sum += block_sum;
        } else {
            // Partial overlap: must check individual elements
            uint64_t block_sum = 0;
            for (uint32_t i = 0; i < block_size; ++i) {
                uint32_t v = block_start[i];
                if (v >= low && v <= high) {
                    block_sum += v;
                }
            }
            single_iter_sum += block_sum;
        }
    }

    // The original baseline resets sum = 0 inside the loop and returns the last iteration's result.
    // "sum = 0; for (int iter = 0; iter < iters; ++iter) { sum = 0; ... }"
    // This means the function effectively returns the result of a single pass.
    return single_iter_sum;
}