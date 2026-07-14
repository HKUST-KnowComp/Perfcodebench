#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const uint64_t offset = 0x9E3779B1LL;
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    
    size_t i = 0;
    size_t limit = count - (count % 4);
    
    for (; i < limit; i += 4) {
      current += (int64_t)deltas[i];
      sum0 += (uint64_t)(current + offset);
      
      current += (int64_t)deltas[i + 1];
      sum1 += (uint64_t)(current + offset);
      
      current += (int64_t)deltas[i + 2];
      sum2 += (uint64_t)(current + offset);
      
      current += (int64_t)deltas[i + 3];
      sum3 += (uint64_t)(current + offset);
    }
    
    for (; i < count; ++i) {
      current += (int64_t)deltas[i];
      sum0 += (uint64_t)(current + offset);
    }
    
    total = sum0 + sum1 + sum2 + sum3;
  }
  
  return total;
}