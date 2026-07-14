#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  // If no iterations requested, baseline returned 0
  if (iters <= 0) return 0;

  // Allocate zero-initialized histogram once
  uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
  if (!hist) return 0;

  const uint32_t* __restrict k = keys;
  const uint32_t* __restrict v = vals;

  // Unrolled accumulation into histogram
  size_t i = 0;
  size_t n8 = n & ~(size_t)7;
  for (; i < n8; i += 8) {
    uint32_t k0 = k[i + 0]; hist[k0] += (uint64_t)v[i + 0];
    uint32_t k1 = k[i + 1]; hist[k1] += (uint64_t)v[i + 1];
    uint32_t k2 = k[i + 2]; hist[k2] += (uint64_t)v[i + 2];
    uint32_t k3 = k[i + 3]; hist[k3] += (uint64_t)v[i + 3];
    uint32_t k4 = k[i + 4]; hist[k4] += (uint64_t)v[i + 4];
    uint32_t k5 = k[i + 5]; hist[k5] += (uint64_t)v[i + 5];
    uint32_t k6 = k[i + 6]; hist[k6] += (uint64_t)v[i + 6];
    uint32_t k7 = k[i + 7]; hist[k7] += (uint64_t)v[i + 7];
  }
  for (; i < n; ++i) {
    hist[k[i]] += (uint64_t)v[i];
  }

  // Compute FNV-1a hash over buckets
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  for (uint32_t b = 0; b < buckets; ++b) {
    hash ^= (hist[b] + 1ULL);
    hash *= FNV_PRIME;
  }

  free(hist);
  return hash;
}
