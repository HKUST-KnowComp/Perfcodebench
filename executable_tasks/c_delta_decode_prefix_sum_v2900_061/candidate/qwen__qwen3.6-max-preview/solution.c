#include "interface.h"
#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  uint64_t bias_sum = count * 0x9E3779B1ULL;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t iter_total = bias_sum;
    for (size_t i = 0; i < count; ++i) {
      current += deltas[i];
      iter_total += (uint64_t)current;
    }
    total = iter_total;
  }
  return total;
}