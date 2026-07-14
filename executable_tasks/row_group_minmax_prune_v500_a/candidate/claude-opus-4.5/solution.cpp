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
      const uint32_t block_min = minsptr[b];
      const uint32_t block_max = maxsptr[b];
      
      // Prune: block entirely outside range
      if (block_max < low || block_min > high) {
        continue;
      }
      
      const size_t start = b * block_size;
      const size_t end = (start + block_size < n) ? (start + block_size) : n;
      
      // Include entire block: all values in range
      if (block_min >= low && block_max <= high) {
        uint64_t block_sum = 0;
        const uint32_t* __restrict p = vptr + start;
        const size_t count = end - start;
        size_t i = 0;
        
        // Unrolled loop for full block sum
        for (; i + 8 <= count; i += 8) {
          block_sum += p[i] + p[i+1] + p[i+2] + p[i+3] +
                       p[i+4] + p[i+5] + p[i+6] + p[i+7];
        }
        for (; i < count; ++i) {
          block_sum += p[i];
        }
        sum += block_sum;
      } else {
        // Partial overlap: must check each value
        uint64_t block_sum = 0;
        const uint32_t* __restrict p = vptr + start;
        const size_t count = end - start;
        size_t i = 0;
        
        // Unrolled loop with branchless accumulation
        for (; i + 4 <= count; i += 4) {
          uint32_t v0 = p[i];
          uint32_t v1 = p[i+1];
          uint32_t v2 = p[i+2];
          uint32_t v3 = p[i+3];
          block_sum += (v0 >= low && v0 <= high) ? v0 : 0;
          block_sum += (v1 >= low && v1 <= high) ? v1 : 0;
          block_sum += (v2 >= low && v2 <= high) ? v2 : 0;
          block_sum += (v3 >= low && v3 <= high) ? v3 : 0;
        }
        for (; i < count; ++i) {
          uint32_t v = p[i];
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