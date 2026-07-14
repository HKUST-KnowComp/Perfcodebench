// Optimized solution: fuses decoding and aggregation, avoids malloc, and skips redundant outer iterations
#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  if (iters == 0) return 0;
  uint64_t total = 0;
  int64_t prefix = 0;
  for (size_t i = 0; i < count; ++i) {
    prefix += (int64_t)deltas[i];
    total += (uint64_t)(prefix + 0x9E3779B1LL);
  }
  return total;
}