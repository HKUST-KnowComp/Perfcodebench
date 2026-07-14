#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* __restrict__ keys_ptr = keys.data();
  const uint8_t* __restrict__ flags_ptr = flags.data();
  const uint32_t* __restrict__ payloads_ptr = payloads.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Unroll by 4 for better ILP and vectorization
    std::size_t i = 0;
    const std::size_t n_unroll = n - (n % 4);
    
    for (; i < n_unroll; i += 4) {
      uint32_t k0 = keys_ptr[i];
      uint32_t k1 = keys_ptr[i + 1];
      uint32_t k2 = keys_ptr[i + 2];
      uint32_t k3 = keys_ptr[i + 3];
      
      uint8_t f0 = flags_ptr[i];
      uint8_t f1 = flags_ptr[i + 1];
      uint8_t f2 = flags_ptr[i + 2];
      uint8_t f3 = flags_ptr[i + 3];
      
      bool cond0 = f0 && (k0 < threshold) && ((k0 & 7u) == 0u);
      bool cond1 = f1 && (k1 < threshold) && ((k1 & 7u) == 0u);
      bool cond2 = f2 && (k2 < threshold) && ((k2 & 7u) == 0u);
      bool cond3 = f3 && (k3 < threshold) && ((k3 & 7u) == 0u);
      
      if (__builtin_expect(cond0, 0)) sum += payloads_ptr[i];
      if (__builtin_expect(cond1, 0)) sum += payloads_ptr[i + 1];
      if (__builtin_expect(cond2, 0)) sum += payloads_ptr[i + 2];
      if (__builtin_expect(cond3, 0)) sum += payloads_ptr[i + 3];
    }
    
    // Handle remainder
    for (; i < n; ++i) {
      uint32_t k = keys_ptr[i];
      if (__builtin_expect(flags_ptr[i] && (k < threshold) && ((k & 7u) == 0u), 0)) {
        sum += payloads_ptr[i];
      }
    }
  }
  
  return sum;
}