#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  const int64_t C = 0x9E3779B1LL;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t t = 0;

    const int32_t* p = deltas;
    size_t n4 = count / 4;

    while (n4--) {
      current += (int64_t)p[0];
      t += (uint64_t)(current + C);
      current += (int64_t)p[1];
      t += (uint64_t)(current + C);
      current += (int64_t)p[2];
      t += (uint64_t)(current + C);
      current += (int64_t)p[3];
      t += (uint64_t)(current + C);
      p += 4;
    }

    switch (count & 3) {
      case 3:
        current += (int64_t)p[0];
        t += (uint64_t)(current + C);
        current += (int64_t)p[1];
        t += (uint64_t)(current + C);
        current += (int64_t)p[2];
        t += (uint64_t)(current + C);
        break;
      case 2:
        current += (int64_t)p[0];
        t += (uint64_t)(current + C);
        current += (int64_t)p[1];
        t += (uint64_t)(current + C);
        break;
      case 1:
        current += (int64_t)p[0];
        t += (uint64_t)(current + C);
        break;
      default:
        break;
    }

    total = t;  // return last iteration's total
  }

  return total;
}
