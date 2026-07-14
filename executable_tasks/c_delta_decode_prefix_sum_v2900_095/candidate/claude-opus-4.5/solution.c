#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  if (count == 0) return 0;
  
  const int64_t constant = 0x9E3779B1LL;
  uint64_t total = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    int64_t prefix_sum = 0;
    
    size_t i = 0;
    size_t unroll = count & ~(size_t)3;
    
    for (; i < unroll; i += 4) {
      current += (int64_t)deltas[i];
      prefix_sum += current;
      current += (int64_t)deltas[i + 1];
      prefix_sum += current;
      current += (int64_t)deltas[i + 2];
      prefix_sum += current;
      current += (int64_t)deltas[i + 3];
      prefix_sum += current;
    }
    
    for (; i < count; ++i) {
      current += (int64_t)deltas[i];
      prefix_sum += current;
    }
    
    total = (uint64_t)prefix_sum + (uint64_t)count * (uint64_t)constant;
  }
  
  return total;
}