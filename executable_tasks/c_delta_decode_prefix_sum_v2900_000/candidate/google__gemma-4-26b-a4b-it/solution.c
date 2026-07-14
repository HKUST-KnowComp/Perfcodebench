#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const int64_t constant = 0x9E3779B1LL;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current_prefix_sum = 0;
    uint64_t iter_total = 0;
    
    // Single pass: compute prefix sum and accumulate the required value immediately.
    // This avoids the O(N) allocation and the second loop, maximizing cache efficiency.
    for (size_t i = 0; i < count; ++i) {
      current_prefix_sum += (int64_t)deltas[i];
      iter_total += (uint64_t)(current_prefix_sum + constant);
    }
    total = iter_total;
  }

  return total;
}