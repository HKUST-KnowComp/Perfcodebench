#include "interface.h"

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
  const size_t num_blocks = mins.size();
  const size_t num_values = values.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t b = 0; b < num_blocks; ++b) {
      uint32_t b_min = min_ptr[b];
      uint32_t b_max = max_ptr[b];

      // Case 1: Block is entirely outside the range
      if (b_max < low || b_min > high) {
        continue;
      }

      // Case 2: Block is entirely within the range
      if (b_min >= low && b_max <= high) {
        size_t start_idx = b * block_size;
        size_t end_idx = (b + 1) * block_size;
        if (end_idx > num_values) end_idx = num_values;
        
        for (size_t i = start_idx; i < end_idx; ++i) {
          current_iter_sum += static_cast<uint64_t>(val_ptr[i]);
        }
      } 
      // Case 3: Block partially overlaps
      else {
        size_t start_idx = b * block_size;
        size_t end_idx = (b + 1) * block_size;
        if (end_idx > num_values) end_idx = num_values;

        for (size_t i = start_idx; i < end_idx; ++i) {
          uint32_t v = val_ptr[i];
          if (v >= low && v <= high) {
            current_iter_sum += static_cast<uint64_t>(v);
          }
        }
      }
    }
    // Handle any trailing values not covered by blocks if block_size doesn't align
    size_t last_block_end = num_blocks * block_size;
    if (last_block_end < num_values) {
        for (size_t i = last_block_end; i < num_values; ++i) {
            uint32_t v = val_ptr[i];
            if (v >= low && v <= high) {
                current_iter_sum += static_cast<uint64_t>(v);
            }
        }
    }

    total_sum = current_iter_sum;
  }
  return total_sum;
}