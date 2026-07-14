#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  // The original code allocates 'values' and performs two passes per iteration.
  // Pass 1: values[i] = prefix_sum(deltas[0...i])
  // Pass 2: total += values[i] + constant
  // We can fuse these into a single pass to avoid O(N) memory allocation and extra cache misses.
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current_prefix_sum = 0;
    uint64_t iteration_total = 0;
    
    for (size_t i = 0; i < count; ++i) {
      current_prefix_sum += (int64_t)deltas[i];
      iteration_total += (uint64_t)(current_prefix_sum + 0x9E3779B1LL);
    }
    total = iteration_total;
  }
  
  return total;
}