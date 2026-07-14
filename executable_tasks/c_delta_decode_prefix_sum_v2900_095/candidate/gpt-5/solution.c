#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const int64_t addC = 0x9E3779B1LL;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    total = 0;

    const int32_t* p = deltas;
    size_t n = count;

    while (n >= 8) {
      current += (int64_t)p[0]; total += (uint64_t)(current + addC);
      current += (int64_t)p[1]; total += (uint64_t)(current + addC);
      current += (int64_t)p[2]; total += (uint64_t)(current + addC);
      current += (int64_t)p[3]; total += (uint64_t)(current + addC);
      current += (int64_t)p[4]; total += (uint64_t)(current + addC);
      current += (int64_t)p[5]; total += (uint64_t)(current + addC);
      current += (int64_t)p[6]; total += (uint64_t)(current + addC);
      current += (int64_t)p[7]; total += (uint64_t)(current + addC);
      p += 8;
      n -= 8;
    }
    while (n--) {
      current += (int64_t)*p++;
      total += (uint64_t)(current + addC);
    }
  }
  return total;
}
