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
  
  const uint32_t* __restrict__ probe_ptr = probe_keys.data();
  const uint32_t* __restrict__ values_ptr = values.data();
  const size_t n = probe_keys.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear bitmap
    std::memset(bitmap.data(), 0, num_words * sizeof(uint64_t));
    
    // Set enabled keys in bitmap
    for (uint32_t key : enabled_keys) {
      bitmap[key >> 6] |= (1ULL << (key & 63));
    }
    
    sum = 0;
    
    // Process 4 elements at a time with branchless adds
    size_t i = 0;
    const size_t n4 = n & ~size_t(3);
    
    for (; i < n4; i += 4) {
      uint32_t k0 = probe_ptr[i];
      uint32_t k1 = probe_ptr[i + 1];
      uint32_t k2 = probe_ptr[i + 2];
      uint32_t k3 = probe_ptr[i + 3];
      
      uint64_t m0 = (bitmap[k0 >> 6] >> (k0 & 63)) & 1;
      uint64_t m1 = (bitmap[k1 >> 6] >> (k1 & 63)) & 1;
      uint64_t m2 = (bitmap[k2 >> 6] >> (k2 & 63)) & 1;
      uint64_t m3 = (bitmap[k3 >> 6] >> (k3 & 63)) & 1;
      
      sum += m0 * values_ptr[i];
      sum += m1 * values_ptr[i + 1];
      sum += m2 * values_ptr[i + 2];
      sum += m3 * values_ptr[i + 3];
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t k = probe_ptr[i];
      uint64_t m = (bitmap[k >> 6] >> (k & 63)) & 1;
      sum += m * values_ptr[i];
    }
  }
  
  return sum;
}
