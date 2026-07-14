#include "interface.h"

#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  (void)iters;
  uint64_t total = 0;
  int64_t current = 0;
  for (size_t i = 0; i < count; ++i) {
    current += deltas[i];
    total += (uint64_t)(current + 0x9E3779B1LL);
  }
  return total;
}
