#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) return 0;
  
  uint64_t hash = 0;
  size_t hist_bytes = buckets * sizeof(uint64_t);
  
  const uint32_t* __restrict__ r_keys = keys;
  const uint32_t* __restrict__ r_vals = vals;
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    for (; i + 3 < n; i += 4) {
      hist[r_keys[i]]   += r_vals[i];
      hist[r_keys[i+1]] += r_vals[i+1];
      hist[r_keys[i+2]] += r_vals[i+2];
      hist[r_keys[i+3]] += r_vals[i+3];
    }
    for (; i < n; ++i) {
      hist[r_keys[i]] += r_vals[i];
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < buckets; ++b) {
      hash ^= (hist[b] + 1ULL);
      hash *= 1099511628211ULL;
    }
    
    if (iter < iters - 1) {
      memset(hist, 0, hist_bytes);
    }
  }
  
  free(hist);
  return hash;
}