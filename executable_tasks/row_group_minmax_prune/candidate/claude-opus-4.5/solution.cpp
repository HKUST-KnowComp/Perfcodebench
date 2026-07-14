#include "interface.h"

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  
  const size_t n = values.size();
  const size_t num_blocks = mins.size();
  const uint32_t* __restrict vptr = values.data();
  const uint32_t* __restrict minsptr = mins.data();
  const uint32_t* __restrict maxsptr = maxs.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t b = 0; b < num_blocks; ++b) {
      uint32_t block_min = minsptr[b];
      uint32_t block_max = maxsptr[b];
      
      // Skip block entirely if no overlap with [low, high]
      if (block_max < low || block_min > high) {
        continue;
      }
      
      size_t start = b * block_size;
      size_t end = start + block_size;
      if (end > n) end = n;
      
      // Include entire block if fully within [low, high]
      if (block_min >= low && block_max <= high) {
        uint64_t block_sum = 0;
        size_t i = start;
        // Unroll loop for better performance
        size_t limit = start + ((end - start) & ~3ULL);
        for (; i < limit; i += 4) {
          block_sum += vptr[i] + vptr[i+1] + vptr[i+2] + vptr[i+3];
        }
        for (; i < end; ++i) {
          block_sum += vptr[i];
        }
        sum += block_sum;
      } else {
        // Partial overlap - need to check each element
        uint64_t block_sum = 0;
        for (size_t i = start; i < end; ++i) {
          uint32_t v = vptr[i];
          if (v >= low && v <= high) {
            block_sum += v;
          }
        }
        sum += block_sum;
      }
    }
  }
  
  return sum;
}