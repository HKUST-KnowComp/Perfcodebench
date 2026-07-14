#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const uint64_t c_total = (uint64_t)count * 0x9E3779B1ULL;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t prefix_sum = 0;
    for (size_t i = 0; i < count; ++i) {
      current += deltas[i];
      prefix_sum += (uint64_t)current;
    }
    total = prefix_sum + c_total;
  }
  return total;
}