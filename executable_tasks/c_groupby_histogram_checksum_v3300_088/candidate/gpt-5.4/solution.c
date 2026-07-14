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
    size_t i = 0;

    size_t n4 = n & ~(size_t)3;
    for (; i < n4; i += 4) {
      hist[k[0]] += v[0];
      hist[k[1]] += v[1];
      hist[k[2]] += v[2];
      hist[k[3]] += v[3];
      k += 4;
      v += 4;
    }
    for (; i < n; ++i) {
      hist[*k++] += *v++;
    }

    hash = 1469598103934665603ULL;
    uint32_t b = 0;
    uint32_t b4 = buckets & ~((uint32_t)3);
    for (; b < b4; b += 4) {
      hash ^= (hist[b + 0] + 1ULL);
      hash *= 1099511628211ULL;
      hash ^= (hist[b + 1] + 1ULL);
      hash *= 1099511628211ULL;
      hash ^= (hist[b + 2] + 1ULL);
      hash *= 1099511628211ULL;
      hash ^= (hist[b + 3] + 1ULL);
      hash *= 1099511628211ULL;
    }
    for (; b < buckets; ++b) {
      hash ^= (hist[b] + 1ULL);
      hash *= 1099511628211ULL;
    }

    if (iter + 1 < iters) {
      for (uint32_t j = 0; j < buckets; ++j) hist[j] = 0;
    }
  }

  free(hist);
  return hash;
}
