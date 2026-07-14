#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const uint64_t magic = 0x9E3779B1ULL;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t iter_sum = 0;
    
    for (size_t i = 0; i < count; ++i) {
      current += (int64_t)deltas[i];
      // The problem requires the sum of (values[i] + 0x9E3779B1LL)
      // Since values[i] is int64_t, we cast to uint64_t to handle the addition
      // and the potential overflow/wrap-around behavior correctly.
      iter_sum += (uint64_t)((int64_t)current + (int64_t)magic);
    }
    total = iter_sum;
  }
  return total;
}