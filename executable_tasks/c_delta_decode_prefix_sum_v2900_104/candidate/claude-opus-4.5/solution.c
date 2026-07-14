#include "interface.h"

#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const int64_t offset = 0x9E3779B1LL;
  const uint64_t offset_sum = (uint64_t)count * (uint64_t)offset;
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum = 0;
    size_t i = 0;
    
    /* Process 4 elements at a time for better ILP */
    size_t limit = count & ~(size_t)3;
    for (; i < limit; i += 4) {
      int64_t d0 = deltas[i];
      int64_t d1 = deltas[i + 1];
      int64_t d2 = deltas[i + 2];
      int64_t d3 = deltas[i + 3];
      
      int64_t v0 = current + d0;
      int64_t v1 = v0 + d1;
      int64_t v2 = v1 + d2;
      int64_t v3 = v2 + d3;
      
      sum += (uint64_t)v0 + (uint64_t)v1 + (uint64_t)v2 + (uint64_t)v3;
      current = v3;
    }
    
    /* Handle remaining elements */
    for (; i < count; ++i) {
      current += deltas[i];
      sum += (uint64_t)current;
    }
    
    total = sum + offset_sum;
  }
  
  return total;
}