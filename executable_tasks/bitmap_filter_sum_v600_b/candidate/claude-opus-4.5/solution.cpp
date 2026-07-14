#include "interface.h"

#include <cstring>
#include <vector>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  
  const size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
  std::vector<uint64_t> bitmap(num_words, 0);
  
  // Build bitmap once - enabled_keys doesn't change between iterations
  for (uint32_t k : enabled_keys) {
    bitmap[k >> 6] |= (1ULL << (k & 63));
  }
  
  const uint64_t* __restrict bm = bitmap.data();
  const uint32_t* __restrict pk = probe_keys.data();
  const uint32_t* __restrict vals = values.data();
  const size_t n = probe_keys.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    size_t i = 0;
    
    // Process 4 elements at a time
    const size_t n4 = n & ~3ULL;
    for (; i < n4; i += 4) {
      uint32_t k0 = pk[i];
      uint32_t k1 = pk[i + 1];
      uint32_t k2 = pk[i + 2];
      uint32_t k3 = pk[i + 3];
      
      uint64_t m0 = (bm[k0 >> 6] >> (k0 & 63)) & 1;
      uint64_t m1 = (bm[k1 >> 6] >> (k1 & 63)) & 1;
      uint64_t m2 = (bm[k2 >> 6] >> (k2 & 63)) & 1;
      uint64_t m3 = (bm[k3 >> 6] >> (k3 & 63)) & 1;
      
      s0 += m0 * vals[i];
      s1 += m1 * vals[i + 1];
      s2 += m2 * vals[i + 2];
      s3 += m3 * vals[i + 3];
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      uint32_t k = pk[i];
      uint64_t m = (bm[k >> 6] >> (k & 63)) & 1;
      s0 += m * vals[i];
    }
    
    sum = s0 + s1 + s2 + s3;
  }
  
  return sum;
}