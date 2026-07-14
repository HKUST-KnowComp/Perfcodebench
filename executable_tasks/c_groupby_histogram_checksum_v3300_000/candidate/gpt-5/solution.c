#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t hash = 0;
  if (iters <= 0) return hash;

  // Allocate histogram once and reuse across iterations
  uint64_t* hist = (uint64_t*)malloc((size_t)buckets * sizeof(uint64_t));
  if (!hist) return 0;

  // Initialize histogram to zero before first use
  for (uint32_t i = 0; i < buckets; ++i) hist[i] = 0;

  const uint32_t* k = keys;
  const uint32_t* v = vals;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate values into histogram (unrolled by 4)
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

    // Compute checksum and clear histogram for next iteration
    hash = FNV_OFFSET;
    for (uint32_t b = 0; b < buckets; ++b) {
      uint64_t hv = hist[b];
      hash ^= (hv + 1ULL);
      hash *= FNV_PRIME;
      hist[b] = 0;
    }
  }

  free(hist);
  return hash;
}
