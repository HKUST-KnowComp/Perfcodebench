#include "interface.h"

#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum = 0;
    
    size_t i = 0;
    size_t limit = count & ~3ULL;
    
    // Unrolled loop with 4 independent accumulator chains
    int64_t curr0 = 0, curr1 = 0, curr2 = 0, curr3 = 0;
    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    
    for (; i < limit; i += 4) {
      curr0 += (int64_t)deltas[i];
      sum0 += (uint64_t)(curr0 + 0x9E3779B1LL);
      
      curr1 = curr0 + (int64_t)deltas[i + 1];
      sum1 += (uint64_t)(curr1 + 0x9E3779B1LL);
      
      curr2 = curr1 + (int64_t)deltas[i + 2];
      sum2 += (uint64_t)(curr2 + 0x9E3779B1LL);
      
      curr3 = curr2 + (int64_t)deltas[i + 3];
      sum3 += (uint64_t)(curr3 + 0x9E3779B1LL);
      
      curr0 = curr3;
    }
    
    current = curr0;
    sum = sum0 + sum1 + sum2 + sum3;
    
    // Handle remaining elements
    for (; i < count; ++i) {
      current += (int64_t)deltas[i];
      sum += (uint64_t)(current + 0x9E3779B1LL);
    }
    
    total = sum;
  }
  
  return total;
}