#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const uint64_t magic = 0x9E3779B1LL;
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t prefix = 0;
    uint64_t sum = 0;
    
    size_t i = 0;
    size_t unroll_limit = count - (count % 4);
    
    // Unrolled loop with multiple accumulators
    for (; i < unroll_limit; i += 4) {
      int64_t p0 = prefix + (int64_t)deltas[i];
      int64_t p1 = p0 + (int64_t)deltas[i + 1];
      int64_t p2 = p1 + (int64_t)deltas[i + 2];
      int64_t p3 = p2 + (int64_t)deltas[i + 3];
      
      sum += (uint64_t)(p0 + magic);
      sum += (uint64_t)(p1 + magic);
      sum += (uint64_t)(p2 + magic);
      sum += (uint64_t)(p3 + magic);
      
      prefix = p3;
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
      prefix += (int64_t)deltas[i];
      sum += (uint64_t)(prefix + magic);
    }
    
    total = sum;
  }
  
  return total;
}