#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    for (; i + 7 < n; i += 8) {
      hist[keys[i]]   += vals[i];
      hist[keys[i+1]] += vals[i+1];
      hist[keys[i+2]] += vals[i+2];
      hist[keys[i+3]] += vals[i+3];
      hist[keys[i+4]] += vals[i+4];
      hist[keys[i+5]] += vals[i+5];
      hist[keys[i+6]] += vals[i+6];
      hist[keys[i+7]] += vals[i+7];
    }
    for (; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t j = 0; j < buckets; ++j) {
      uint64_t v = hist[j];
      hist[j] = 0;
      hash ^= (v + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  free(hist);
  return hash;
}