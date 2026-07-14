#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (iters <= 0) return 0;

  // Allocate histogram once and reuse across iterations.
  uint64_t* hist = (uint64_t*)calloc((size_t)buckets, sizeof(uint64_t));
  if (!hist) return 0;

  const uint32_t* restrict k = keys;
  const uint32_t* restrict v = vals;

  uint64_t hash = 0;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME  = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    // Aggregate values into the histogram (unrolled by 4).
    size_t i = 0;
    size_t n4 = n & ~(size_t)3;
    for (; i < n4; i += 4) {
      hist[k[i + 0]] += v[i + 0];
      hist[k[i + 1]] += v[i + 1];
      hist[k[i + 2]] += v[i + 2];
      hist[k[i + 3]] += v[i + 3];
    }
    for (; i < n; ++i) {
      hist[k[i]] += v[i];
    }

    // Compute checksum and simultaneously clear histogram for next iteration.
    uint64_t h = FNV_OFFSET;
    uint32_t b = 0;
    uint32_t b4 = buckets & ~3u;
    for (; b < b4; b += 4) {
      uint64_t s0 = hist[b + 0]; hist[b + 0] = 0;
      h ^= (s0 + 1ULL);
      h *= FNV_PRIME;
      uint64_t s1 = hist[b + 1]; hist[b + 1] = 0;
      h ^= (s1 + 1ULL);
      h *= FNV_PRIME;
      uint64_t s2 = hist[b + 2]; hist[b + 2] = 0;
      h ^= (s2 + 1ULL);
      h *= FNV_PRIME;
      uint64_t s3 = hist[b + 3]; hist[b + 3] = 0;
      h ^= (s3 + 1ULL);
      h *= FNV_PRIME;
    }
    for (; b < buckets; ++b) {
      uint64_t s = hist[b]; hist[b] = 0;
      h ^= (s + 1ULL);
      h *= FNV_PRIME;
    }

    hash = h;
  }

  free(hist);
  return hash;
}
