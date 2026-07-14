#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (iters <= 0) return 0;

  uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
  if (!hist) return 0;

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t* k = keys;
    const uint32_t* v = vals;
    const uint32_t* kend = keys + n;

    while (k != kend) {
      hist[*k++] += (uint64_t)(*v++);
    }

    hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      hash ^= (hist[i] + 1ULL);
      hash *= 1099511628211ULL;
    }

    if (iter + 1 < iters) {
      uint64_t* p = hist;
      uint64_t* end = hist + buckets;
      while (p != end) {
        *p++ = 0;
      }
    }
  }

  free(hist);
  return hash;
}
