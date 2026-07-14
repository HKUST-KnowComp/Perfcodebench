#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  uint64_t final_hash = 0;
  if (iters <= 0 || buckets == 0) return final_hash;
  
  // Allocate histogram once, aligned to 64-byte cache line for optimal CPU access
  uint64_t* hist = (uint64_t*)aligned_alloc(64, buckets * sizeof(uint64_t));
  if (!hist) return 0;
  
  // Zero histogram a single time at initialization
  memset(hist, 0, buckets * sizeof(uint64_t));
  
  for (int iter = 0; iter < iters; ++iter) {
    // Aggregate values into histogram, identical order to baseline
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }
    
    // Compute FNV-1a hash exactly as baseline, reset buckets during the pass
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      const uint64_t current = hist[i];
      hash ^= (current + 1ULL);
      hash *= 1099511628211ULL;
      hist[i] = 0; // Reset for next iteration, no extra zero pass needed
    }
    final_hash = hash;
  }
  
  free(hist);
  return final_hash;
}