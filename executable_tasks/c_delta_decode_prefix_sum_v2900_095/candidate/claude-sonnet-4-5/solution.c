#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum = 0;
    
    size_t i = 0;
    size_t unroll_limit = count - (count % 4);
    
    // Unrolled loop with multiple accumulators
    for (; i < unroll_limit; i += 4) {
      int64_t v0 = current + (int64_t)deltas[i];
      int64_t v1 = v0 + (int64_t)deltas[i + 1];
      int64_t v2 = v1 + (int64_t)deltas[i + 2];
      int64_t v3 = v2 + (int64_t)deltas[i + 3];
      
      sum += (uint64_t)(v0 + 0x9E3779B1LL);
      sum += (uint64_t)(v1 + 0x9E3779B1LL);
      sum += (uint64_t)(v2 + 0x9E3779B1LL);
      sum += (uint64_t)(v3 + 0x9E3779B1LL);
      
      current = v3;
    }
    
    // Handle remaining elements
    for (; i < count; ++i) {
      current += (int64_t)deltas[i];
      sum += (uint64_t)(current + 0x9E3779B1LL);
    }
    
    total = sum;
  }
  
  return total;
}