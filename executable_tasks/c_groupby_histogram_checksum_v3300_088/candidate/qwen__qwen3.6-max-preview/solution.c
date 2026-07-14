#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  const uint32_t* restrict k = keys;
  const uint32_t* restrict v = vals;
  size_t b_stride = buckets;
  uint64_t* hist = (uint64_t*)malloc(b_stride * 4 * sizeof(uint64_t));
  if (!hist) return 0;

  uint64_t hash = 0;
  size_t hist_bytes = b_stride * 4 * sizeof(uint64_t);

  for (int iter = 0; iter < iters; ++iter) {
    memset(hist, 0, hist_bytes);

    size_t i = 0;
    for (; i + 3 < n; i += 4) {
      hist[k[i]] += v[i];
      hist[k[i+1] + b_stride] += v[i+1];
      hist[k[i+2] + b_stride*2] += v[i+2];
      hist[k[i+3] + b_stride*3] += v[i+3];
    }
    for (; i < n; ++i) {
      hist[k[i] + (i & 3) * b_stride] += v[i];
    }

    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < buckets; ++b) {
      uint64_t sum = hist[b] + hist[b + b_stride] + hist[b + b_stride*2] + hist[b + b_stride*3];
      hash ^= (sum + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  free(hist);
  return hash;
}