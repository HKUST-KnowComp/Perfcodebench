#include "interface.h"
#include <cstdint>

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
  const uint32_t* __restrict minptr = mins.data();
  const uint32_t* __restrict maxptr = maxs.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t b = 0; b < num_blocks; ++b) {
      uint32_t block_min = minptr[b];
      uint32_t block_max = maxptr[b];
      
      // Skip block if entirely outside range
      if (block_max < low || block_min > high) {
        continue;
      }
      
      size_t start = b * block_size;
      size_t end = start + block_size;
      if (end > n) end = n;
      
      // Include entire block if fully within range
      if (block_min >= low && block_max <= high) {
        uint64_t block_sum = 0;
        size_t i = start;
        
        // Unroll by 8 for better ILP
        size_t unroll_end = start + ((end - start) & ~7ULL);
        for (; i < unroll_end; i += 8) {
          block_sum += vptr[i] + vptr[i+1] + vptr[i+2] + vptr[i+3]
                     + vptr[i+4] + vptr[i+5] + vptr[i+6] + vptr[i+7];
        }
        for (; i < end; ++i) {
          block_sum += vptr[i];
        }
        sum += block_sum;
      } else {
        // Partial block - need to check each element
        uint64_t block_sum = 0;
        size_t i = start;
        
        // Unroll by 4 for partial blocks
        size_t unroll_end = start + ((end - start) & ~3ULL);
        for (; i < unroll_end; i += 4) {
          uint32_t v0 = vptr[i];
          uint32_t v1 = vptr[i+1];
          uint32_t v2 = vptr[i+2];
          uint32_t v3 = vptr[i+3];
          
          if (v0 >= low && v0 <= high) block_sum += v0;
          if (v1 >= low && v1 <= high) block_sum += v1;
          if (v2 >= low && v2 <= high) block_sum += v2;
          if (v3 >= low && v3 <= high) block_sum += v3;
        }
        for (; i < end; ++i) {
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