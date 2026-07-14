#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  if (iters <= 0 || count == 0) {
    return 0;
  }

  const int64_t OFFSET = 0x9E3779B1LL;
  uint64_t total = 0;
  int64_t current = 0;

  const int32_t* p = deltas;
  size_t i = 0;

  // Unroll by 8 for better throughput
  for (; i + 8 <= count; i += 8) {
    current += (int64_t)p[0]; total += (uint64_t)(current + OFFSET);
    current += (int64_t)p[1]; total += (uint64_t)(current + OFFSET);
    current += (int64_t)p[2]; total += (uint64_t)(current + OFFSET);
    current += (int64_t)p[3]; total += (uint64_t)(current + OFFSET);
    current += (int64_t)p[4]; total += (uint64_t)(current + OFFSET);
    current += (int64_t)p[5]; total += (uint64_t)(current + OFFSET);
    current += (int64_t)p[6]; total += (uint64_t)(current + OFFSET);
    current += (int64_t)p[7]; total += (uint64_t)(current + OFFSET);
    p += 8;
  }

  // Remainder
  for (; i < count; ++i) {
    current += (int64_t)*p++;
    total += (uint64_t)(current + OFFSET);
  }

  return total;
}
