#include "interface.h"

#include <cstring>
#include <cstdint>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  
  const size_t num_words = (static_cast<size_t>(key_space) + 63) / 64;
  std::vector<uint64_t> bitmap(num_words);
  
  uint64_t sum = 0;
  const size_t n = probe_keys.size();
  const uint32_t* __restrict pk = probe_keys.data();
  const uint32_t* __restrict vals = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(bitmap.data(), 0, num_words * sizeof(uint64_t));
    
    for (uint32_t key : enabled_keys) {
      bitmap[key >> 6] |= (1ULL << (key & 63));
    }
    
    sum = 0;
    size_t i = 0;
    const size_t n4 = n & ~static_cast<size_t>(3);
    
    for (; i < n4; i += 4) {
      uint32_t k0 = pk[i];
      uint32_t k1 = pk[i + 1];
      uint32_t k2 = pk[i + 2];
      uint32_t k3 = pk[i + 3];
      
      uint64_t m0 = (bitmap[k0 >> 6] >> (k0 & 63)) & 1;
      uint64_t m1 = (bitmap[k1 >> 6] >> (k1 & 63)) & 1;
      uint64_t m2 = (bitmap[k2 >> 6] >> (k2 & 63)) & 1;
      uint64_t m3 = (bitmap[k3 >> 6] >> (k3 & 63)) & 1;
      
      sum += m0 * vals[i];
      sum += m1 * vals[i + 1];
      sum += m2 * vals[i + 2];
      sum += m3 * vals[i + 3];
    }
    
    for (; i < n; ++i) {
      uint32_t k = pk[i];
      uint64_t m = (bitmap[k >> 6] >> (k & 63)) & 1;
      sum += m * vals[i];
    }
  }
  
  return sum;
}