#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  const uint32_t* restrict k = keys;
  const uint32_t* restrict v = vals;
  uint64_t hash = 0;

  if (iters <= 0) return 0;
  if (buckets == 0) {
    // No buckets: mimic baseline behavior of setting hash per iter
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL; (void)FNV_PRIME; // unused but kept for parity
    for (int iter = 0; iter < iters; ++iter) {
      hash = FNV_OFFSET_BASIS;
    }
    return hash;
  }

  uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
  if (!hist) return 0;

  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate values into histogram
    size_t i = 0;
    size_t n4 = n & ~(size_t)3;
    for (; i < n4; i += 4) {
      hist[(size_t)k[i+0]] += (uint64_t)v[i+0];
      hist[(size_t)k[i+1]] += (uint64_t)v[i+1];
      hist[(size_t)k[i+2]] += (uint64_t)v[i+2];
      hist[(size_t)k[i+3]] += (uint64_t)v[i+3];
    }
    for (; i < n; ++i) {
      hist[(size_t)k[i]] += (uint64_t)v[i];
    }

    // Compute checksum while resetting histogram to zero for next iteration
    hash = FNV_OFFSET_BASIS;
    uint32_t b = 0;
    uint32_t b4 = buckets & ~3u;
    for (; b < b4; b += 4) {
      uint64_t x0 = hist[b+0] + 1ULL; hash ^= x0; hash *= FNV_PRIME; hist[b+0] = 0ULL;
      uint64_t x1 = hist[b+1] + 1ULL; hash ^= x1; hash *= FNV_PRIME; hist[b+1] = 0ULL;
      uint64_t x2 = hist[b+2] + 1ULL; hash ^= x2; hash *= FNV_PRIME; hist[b+2] = 0ULL;
      uint64_t x3 = hist[b+3] + 1ULL; hash ^= x3; hash *= FNV_PRIME; hist[b+3] = 0ULL;
    }
    for (; b < buckets; ++b) {
      uint64_t x = hist[b] + 1ULL;
      hash ^= x;
      hash *= FNV_PRIME;
      hist[b] = 0ULL;
    }
  }

  free(hist);
  return hash;
}
