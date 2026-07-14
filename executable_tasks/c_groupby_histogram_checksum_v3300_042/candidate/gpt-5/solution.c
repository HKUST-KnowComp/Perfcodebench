#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  // If no iterations, match baseline behavior: return 0 without allocating.
  if (iters <= 0) return 0;

  uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
  if (!hist) return 0;

  const uint32_t* k = keys;
  const uint32_t* v = vals;
  const uint32_t B = buckets;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate values into histogram (unrolled by 8)
    size_t i = 0;
    uint64_t* h = hist;
    size_t nn = n;
    for (; i + 7 < nn; i += 8) {
      uint32_t k0 = k[i + 0]; h[k0] += (uint64_t)v[i + 0];
      uint32_t k1 = k[i + 1]; h[k1] += (uint64_t)v[i + 1];
      uint32_t k2 = k[i + 2]; h[k2] += (uint64_t)v[i + 2];
      uint32_t k3 = k[i + 3]; h[k3] += (uint64_t)v[i + 3];
      uint32_t k4 = k[i + 4]; h[k4] += (uint64_t)v[i + 4];
      uint32_t k5 = k[i + 5]; h[k5] += (uint64_t)v[i + 5];
      uint32_t k6 = k[i + 6]; h[k6] += (uint64_t)v[i + 6];
      uint32_t k7 = k[i + 7]; h[k7] += (uint64_t)v[i + 7];
    }
    for (; i < nn; ++i) {
      h[k[i]] += (uint64_t)v[i];
    }

    // Compute FNV-1a checksum and zero histogram in the same pass
    uint64_t hval = 1469598103934665603ULL;      // FNV offset basis
    const uint64_t FNV_PRIME = 1099511628211ULL; // FNV prime

    uint64_t* p = hist;
    uint32_t j = 0;

    // Unroll by 4 for better throughput
    for (; j + 3 < B; j += 4) {
      uint64_t x0 = p[0] + 1ULL; hval ^= x0; hval *= FNV_PRIME; p[0] = 0;
      uint64_t x1 = p[1] + 1ULL; hval ^= x1; hval *= FNV_PRIME; p[1] = 0;
      uint64_t x2 = p[2] + 1ULL; hval ^= x2; hval *= FNV_PRIME; p[2] = 0;
      uint64_t x3 = p[3] + 1ULL; hval ^= x3; hval *= FNV_PRIME; p[3] = 0;
      p += 4;
    }
    for (; j < B; ++j) {
      uint64_t x = *p + 1ULL;
      hval ^= x;
      hval *= FNV_PRIME;
      *p++ = 0;
    }

    hash = hval;
  }

  free(hist);
  return hash;
}
