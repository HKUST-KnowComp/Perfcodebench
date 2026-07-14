#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  if (iters <= 0) return 0;
  const int64_t offset = 0x9E3779B1LL;
  int64_t current = 0;
  uint64_t total = 0;
  for (size_t i = 0; i < count; ++i) {
    current += (int64_t)deltas[i];
    total += (uint64_t)(current + offset);
  }
  return total;
}
