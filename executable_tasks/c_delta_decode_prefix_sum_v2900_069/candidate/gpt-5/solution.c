#include "interface.h"

#include <stdint.h>
#include <stddef.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  const uint64_t C = 0x9E3779B1ULL;
  uint64_t total = 0;

  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t sum = 0;

    const int32_t* p = deltas;
    size_t n = count;

    // Unrolled prefix-sum accumulation (dependency chain maintained)
    while (n >= 4) {
      int64_t c = current;
      c += (int64_t)p[0];
      sum += (uint64_t)c;
      c += (int64_t)p[1];
      sum += (uint64_t)c;
      c += (int64_t)p[2];
      sum += (uint64_t)c;
      c += (int64_t)p[3];
      sum += (uint64_t)c;
      current = c;
      p += 4;
      n -= 4;
    }
    while (n--) {
      current += (int64_t)*p++;
      sum += (uint64_t)current;
    }

    // Add the constant term once per element after the loop
    sum += C * (uint64_t)count;

    // Match baseline semantics: result from the last iteration
    total = sum;
  }

  return total;
}
