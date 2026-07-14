#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const uint64_t constant = 0x9E3779B1ULL;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t prefix = 0;
    uint64_t sum_of_prefixes = 0;
    for (size_t i = 0; i < count; ++i) {
      prefix += (int64_t)deltas[i];
      sum_of_prefixes += (uint64_t)prefix;
    }
    total = sum_of_prefixes + (uint64_t)count * constant;
  }
  return total;
}
