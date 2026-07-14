#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
  if (!hist) return 0;

  uint64_t hash;
  for (int iter = 0; iter < iters; ++iter) {
    memset(hist, 0, buckets * sizeof(uint64_t));

    // Accumulate with manual unrolling (factor 4)
    size_t i = 0;
    size_t limit = n - (n % 4);
    for (; i < limit; i += 4) {
      hist[keys[i]] += vals[i];
      hist[keys[i+1]] += vals[i+1];
      hist[keys[i+2]] += vals[i+2];
      hist[keys[i+3]] += vals[i+3];
    }
    for (; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }

    // Checksum over buckets (unroll factor 4)
    hash = 1469598103934665603ULL;
    uint32_t b = 0;
    uint32_t blimit = buckets - (buckets % 4);
    for (; b < blimit; b += 4) {
      hash ^= (hist[b] + 1ULL);
      hash *= 1099511628211ULL;
      hash ^= (hist[b+1] + 1ULL);
      hash *= 1099511628211ULL;
      hash ^= (hist[b+2] + 1ULL);
      hash *= 1099511628211ULL;
      hash ^= (hist[b+3] + 1ULL);
      hash *= 1099511628211ULL;
    }
    for (; b < buckets; ++b) {
      hash ^= (hist[b] + 1ULL);
      hash *= 1099511628211ULL;
    }
  }

  free(hist);
  return hash;
}