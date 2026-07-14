#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = 0;
  if (iters <= 0) return 0;

  if (buckets == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      hash = FNV_OFFSET_BASIS;
      /* no buckets to process */
    }
    return hash;
  }

  uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
  if (!hist) return 0;

  const uint32_t * restrict k = keys;
  const uint32_t * restrict v = vals;

  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    size_t n4 = n & ~(size_t)3;
    for (; i < n4; i += 4) {
      uint32_t k0 = k[i];
      uint32_t k1 = k[i + 1];
      uint32_t k2 = k[i + 2];
      uint32_t k3 = k[i + 3];
      hist[k0] += (uint64_t)v[i];
      hist[k1] += (uint64_t)v[i + 1];
      hist[k2] += (uint64_t)v[i + 2];
      hist[k3] += (uint64_t)v[i + 3];
    }
    for (; i < n; ++i) {
      hist[k[i]] += (uint64_t)v[i];
    }

    hash = FNV_OFFSET_BASIS;
    uint64_t* p = hist;
    uint64_t* end = hist + (size_t)buckets;
    for (; p != end; ++p) {
      uint64_t val64 = *p;
      hash ^= (val64 + 1ULL);
      hash *= FNV_PRIME;
      *p = 0ULL;  /* prepare for next iteration */
    }
  }

  free(hist);
  return hash;
}
