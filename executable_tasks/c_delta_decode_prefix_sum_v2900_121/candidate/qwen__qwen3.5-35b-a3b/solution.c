#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const uint64_t constant = 0x9E3779B1ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum_prefix = 0;
    for (size_t i = 0; i < count; ++i) {
      sum_prefix += (uint64_t)deltas[i] * (count - i);
    }
    total += sum_prefix + count * constant;
  }
  
  return total;
}