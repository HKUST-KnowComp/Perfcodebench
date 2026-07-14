#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* restrict keys, const uint32_t* restrict vals, size_t n, uint32_t buckets, int iters) {
  uint64_t* hist = (uint64_t*)malloc(buckets * sizeof(uint64_t));
  if (!hist) return 0;
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    memset(hist, 0, buckets * sizeof(uint64_t));
    
    size_t i = 0;
    size_t n_unroll = n - (n % 4);
    for (; i < n_unroll; i += 4) {
      hist[keys[i]] += vals[i];
      hist[keys[i+1]] += vals[i+1];
      hist[keys[i+2]] += vals[i+2];
      hist[keys[i+3]] += vals[i+3];
    }
    for (; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }
    
    hash = 1469598103934665603ULL;
    uint32_t j = 0;
    uint32_t buckets_unroll = buckets - (buckets % 4);
    for (; j < buckets_unroll; j += 4) {
      uint64_t h0 = hash;
      h0 ^= (hist[j] + 1ULL);
      h0 *= 1099511628211ULL;
      h0 ^= (hist[j+1] + 1ULL);
      h0 *= 1099511628211ULL;
      h0 ^= (hist[j+2] + 1ULL);
      h0 *= 1099511628211ULL;
      h0 ^= (hist[j+3] + 1ULL);
      h0 *= 1099511628211ULL;
      hash = h0;
    }
    for (; j < buckets; ++j) {
      hash ^= (hist[j] + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  
  free(hist);
  return hash;
}
