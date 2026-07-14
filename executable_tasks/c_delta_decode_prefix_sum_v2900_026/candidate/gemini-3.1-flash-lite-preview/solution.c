#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  if (count == 0) return 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_total = 0;
    int64_t current = 0;
    
    // Fused prefix sum and aggregation to avoid O(N) memory allocation and storage
    for (size_t i = 0; i < count; ++i) {
      current += (int64_t)deltas[i];
      // The problem requires summing (value + 0x9E3779B1LL)
      // We cast to uint64_t to handle potential negative values correctly as per standard C behavior
      iter_total += (uint64_t)(current + 0x9E3779B1LL);
    }
    total = iter_total;
  }
  return total;
}