#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
    if (iters <= 0 || values.empty()) return 0;
    const uint32_t* val_ptr = values.data();
    const uint32_t* mins_ptr = mins.data();
    const uint32_t* maxs_ptr = maxs.data();
    size_t n_values = values.size();
    size_t num_blocks = mins.size();
    uint64_t total = 0;
    size_t idx = 0;
    for (size_t b = 0; b < num_blocks; ++b) {
        size_t block_end = idx + block_size;
        if (block_end > n_values) block_end = n_values;
        uint32_t b_min = mins_ptr[b];
        uint32_t b_max = maxs_ptr[b];
        if (b_min > high || b_max < low) {
            idx = block_end;
            continue;
        }
        if (b_min >= low && b_max <= high) {
            // full block in range, sum without per-value checks
            for (; idx < block_end; ++idx) {
                total += static_cast<uint64_t>(val_ptr[idx]);
            }
        } else {
            // partial overlap, check each value
            for (; idx < block_end; ++idx) {
                uint32_t v = val_ptr[idx];
                if (v >= low && v <= high) {
                    total += static_cast<uint64_t>(v);
                }
            }
        }
    }
    return total;
}