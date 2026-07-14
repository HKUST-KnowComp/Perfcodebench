#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (iters <= 0) return 0;

  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  size_t B = (size_t)buckets;
  uint64_t* hist = (uint64_t*)calloc(B, sizeof(uint64_t));
  if (!hist) return 0;

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Aggregate
    const uint32_t* restrict ks = keys;
    const uint32_t* restrict vs = vals;
    size_t i = 0;
    size_t n4 = n & ~(size_t)3;
    for (; i < n4; i += 4) {
      uint32_t k0 = ks[i];
      uint32_t k1 = ks[i + 1];
      uint32_t k2 = ks[i + 2];
      uint32_t k3 = ks[i + 3];
      hist[k0] += (uint64_t)vs[i];
      hist[k1] += (uint64_t)vs[i + 1];
      hist[k2] += (uint64_t)vs[i + 2];
      hist[k3] += (uint64_t)vs[i + 3];
    }
    for (; i < n; ++i) {
      hist[ks[i]] += (uint64_t)vs[i];
    }

    // Compute checksum and clear histogram for next iteration in one pass
    hash = FNV_OFFSET;
    size_t b = 0;
    size_t b4 = B & ~(size_t)3;
    for (; b < b4; b += 4) {
      uint64_t v0 = hist[b];
      uint64_t v1 = hist[b + 1];
      uint64_t v2 = hist[b + 2];
      uint64_t v3 = hist[b + 3];

      hash ^= (v0 + 1ULL);
      hash *= FNV_PRIME;
      hash ^= (v1 + 1ULL);
      hash *= FNV_PRIME;
      hash ^= (v2 + 1ULL);
      hash *= FNV_PRIME;
      hash ^= (v3 + 1ULL);
      hash *= FNV_PRIME;

      hist[b] = 0;
      hist[b + 1] = 0;
      hist[b + 2] = 0;
      hist[b + 3] = 0;
    }
    for (; b < B; ++b) {
      uint64_t v = hist[b];
      hash ^= (v + 1ULL);
      hash *= FNV_PRIME;
      hist[b] = 0;
    }
  }

  free(hist);
  return hash;
}
