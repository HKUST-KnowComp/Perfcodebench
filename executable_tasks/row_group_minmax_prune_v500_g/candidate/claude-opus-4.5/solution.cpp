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
  const uint32_t* __restrict minsptr = mins.data();
  const uint32_t* __restrict maxsptr = maxs.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t b = 0; b < num_blocks; ++b) {
      uint32_t block_min = minsptr[b];
      uint32_t block_max = maxsptr[b];
      
      // Skip block if entirely outside range
      if (block_max < low || block_min > high) {
        continue;
      }
      
      size_t start = b * block_size;
      size_t end = start + block_size;
      if (end > n) end = n;
      
      // Include entire block if fully within range
      if (block_min >= low && block_max <= high) {
        const uint32_t* p = vptr + start;
        const uint32_t* pend = vptr + end;
        uint64_t block_sum = 0;
        
        // Process 8 elements at a time
        while (p + 8 <= pend) {
          block_sum += p[0];
          block_sum += p[1];
          block_sum += p[2];
          block_sum += p[3];
          block_sum += p[4];
          block_sum += p[5];
          block_sum += p[6];
          block_sum += p[7];
          p += 8;
        }
        while (p < pend) {
          block_sum += *p++;
        }
        sum += block_sum;
      } else {
        // Partial block - need to check each element
        const uint32_t* p = vptr + start;
        const uint32_t* pend = vptr + end;
        uint64_t block_sum = 0;
        
        // Unrolled loop for better performance
        while (p + 4 <= pend) {
          uint32_t v0 = p[0];
          uint32_t v1 = p[1];
          uint32_t v2 = p[2];
          uint32_t v3 = p[3];
          if (v0 >= low && v0 <= high) block_sum += v0;
          if (v1 >= low && v1 <= high) block_sum += v1;
          if (v2 >= low && v2 <= high) block_sum += v2;
          if (v3 >= low && v3 <= high) block_sum += v3;
          p += 4;
        }
        while (p < pend) {
          uint32_t v = *p++;
          if (v >= low && v <= high) block_sum += v;
        }
        sum += block_sum;
      }
    }
  }
  
  return sum;
}