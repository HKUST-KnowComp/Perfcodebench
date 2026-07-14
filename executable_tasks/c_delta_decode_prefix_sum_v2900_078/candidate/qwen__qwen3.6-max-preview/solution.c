#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  uint64_t const_part = (uint64_t)count * 0x9E3779B1ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_total = const_part;
    for (size_t i = 0; i < count; ++i) {
      iter_total += (uint64_t)((int64_t)(count - i) * deltas[i]);
    }
    total = iter_total;
  }
  return total;
}