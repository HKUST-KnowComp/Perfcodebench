#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  (void)iters;
  uint64_t sum = 0;
  const uint32_t* val_ptr = values.data();
  const uint32_t* min_ptr = mins.data();
  const uint32_t* max_ptr = maxs.data();
  size_t n = values.size();
  size_t num_blocks = mins.size();
  size_t offset = 0;
  
  for (size_t i = 0; i < num_blocks; ++i) {
    if (min_ptr[i] > high || max_ptr[i] < low) {
      offset += block_size;
      continue;
    }
    
    size_t end = offset + block_size;
    if (end > n) end = n;
    
    const uint32_t* p = val_ptr + offset;
    const uint32_t* pend = val_ptr + end;
    for (; p != pend; ++p) {
      uint32_t v = *p;
      if (v >= low && v <= high) {
        sum += v;
      }
    }
    offset = end;
  }
  return sum;
}