#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters) {
  if (buckets == 0 || iters == 0) return 0;
  
  // Align histogram to 64-byte cache line for SIMD compatibility and cache efficiency
  const size_t elem_size = sizeof(uint64_t);
  const size_t raw_bytes = buckets * elem_size;
  const size_t aligned_bytes = (raw_bytes + 63) / 64 * 64; // Round up to 64-byte alignment
  uint64_t* hist = (uint64_t*)aligned_alloc(64, aligned_bytes);
  if (!hist) return 0;
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Zero buffer with optimized memset instead of repeated calloc
    memset(hist, 0, raw_bytes);
    
    // Original aggregation logic preserved for correctness, compiler can vectorize with aligned memory
    for (size_t i = 0; i < n; ++i) {
      hist[keys[i]] += vals[i];
    }
    
    // Exact same FNV-1a checksum calculation as baseline to guarantee matching output
    hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < buckets; ++i) {
      hash ^= (hist[i] + 1ULL);
      hash *= 1099511628211ULL;
    }
  }
  
  free(hist);
  return hash;
}