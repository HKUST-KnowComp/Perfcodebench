#include "interface.h"

#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const int64_t offset = 0x9E3779B1LL;
  const uint64_t offset_sum = (uint64_t)offset * count;
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    int64_t prefix_sum = 0;
    
    size_t i = 0;
    size_t limit = count & ~(size_t)3;
    
    for (; i < limit; i += 4) {
      current += deltas[i];
      int64_t v0 = current;
      current += deltas[i + 1];
      int64_t v1 = current;
      current += deltas[i + 2];
      int64_t v2 = current;
      current += deltas[i + 3];
      int64_t v3 = current;
      prefix_sum += v0 + v1 + v2 + v3;
    }
    
    for (; i < count; ++i) {
      current += deltas[i];
      prefix_sum += current;
    }
    
    total = (uint64_t)prefix_sum + offset_sum;
  }
  return total;
}