#include "interface.h"
#include <stdint.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  const int32_t* restrict d = deltas;
  const int64_t C = 0x9E3779B1LL;
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t cur = 0;
    uint64_t acc = 0;
    for (size_t i = 0; i < count; ++i) {
      cur += (int64_t)d[i];
      acc += (uint64_t)(cur + C);
    }
    total = acc;
  }
  return total;
}
