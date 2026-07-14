#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (iters <= 0) return 0;

  uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
  if (!hist) return 0;

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate into histogram
    const uint32_t* k = keys;
    const uint32_t* v = vals;
    size_t i = 0;
    size_t n4 = n & ~(size_t)3;
    for (; i < n4; i += 4) {
      hist[k[0]] += (uint64_t)v[0];
      hist[k[1]] += (uint64_t)v[1];
      hist[k[2]] += (uint64_t)v[2];
      hist[k[3]] += (uint64_t)v[3];
      k += 4;
      v += 4;
    }
    for (; i < n; ++i) {
      hist[*k++] += (uint64_t)(*v++);
    }

    // Hash the histogram and reset it for next iteration in one pass
    uint64_t h = 1469598103934665603ULL;
    uint32_t b = buckets;
    uint64_t* p = hist;
    while (b--) {
      uint64_t val = *p;
      h ^= (val + 1ULL);
      h *= 1099511628211ULL;
      *p++ = 0ULL;
    }
    hash = h;
  }

  free(hist);
  return hash;
}
