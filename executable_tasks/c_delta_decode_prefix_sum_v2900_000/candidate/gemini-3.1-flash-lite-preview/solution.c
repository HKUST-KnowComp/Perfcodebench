#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const uint64_t magic = 0x9E3779B1ULL;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_total = 0;
    int64_t current = 0;
    for (size_t i = 0; i < count; ++i) {
      current += (int64_t)deltas[i];
      iter_total += (uint64_t)(current + (int64_t)magic);
    }
    total = iter_total;
  }
  return total;
}