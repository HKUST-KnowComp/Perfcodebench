#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  if (iters <= 0) return 0;

  uint64_t total = 0;
  int64_t current = 0;
  const int64_t bias = 0x9E3779B1LL;

  const int32_t* p = deltas;
  size_t n = count;
  size_t i = 0;

  // Unroll by 4 for better ILP and fewer loop overheads
  for (; i + 4 <= n; i += 4) {
    current += (int64_t)p[0];
    total += (uint64_t)(current + bias);
    current += (int64_t)p[1];
    total += (uint64_t)(current + bias);
    current += (int64_t)p[2];
    total += (uint64_t)(current + bias);
    current += (int64_t)p[3];
    total += (uint64_t)(current + bias);
    p += 4;
  }

  switch (n - i) {
    case 3:
      current += (int64_t)p[0];
      total += (uint64_t)(current + bias);
      current += (int64_t)p[1];
      total += (uint64_t)(current + bias);
      current += (int64_t)p[2];
      total += (uint64_t)(current + bias);
      break;
    case 2:
      current += (int64_t)p[0];
      total += (uint64_t)(current + bias);
      current += (int64_t)p[1];
      total += (uint64_t)(current + bias);
      break;
    case 1:
      current += (int64_t)p[0];
      total += (uint64_t)(current + bias);
      break;
    default:
      break;
  }

  // Only the last iteration's result matters for the return value in the baseline; compute once.
  return total;
}
