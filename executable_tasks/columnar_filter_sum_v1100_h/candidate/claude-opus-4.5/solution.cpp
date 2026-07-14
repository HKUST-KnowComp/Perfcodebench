#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  
  const std::size_t n = keys.size();
  if (n == 0) return 0;
  
  const uint32_t* __restrict__ k_ptr = keys.data();
  const uint32_t* __restrict__ v_ptr = values.data();
  const uint8_t* __restrict__ f_ptr = flags.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    std::size_t i = 0;
    const std::size_t unroll_limit = n & ~std::size_t(3);
    
    // Unrolled loop - process 4 elements at a time
    for (; i < unroll_limit; i += 4) {
      uint32_t k0 = k_ptr[i];
      uint32_t k1 = k_ptr[i + 1];
      uint32_t k2 = k_ptr[i + 2];
      uint32_t k3 = k_ptr[i + 3];
      
      uint32_t v0 = v_ptr[i];
      uint32_t v1 = v_ptr[i + 1];
      uint32_t v2 = v_ptr[i + 2];
      uint32_t v3 = v_ptr[i + 3];
      
      uint8_t f0 = f_ptr[i];
      uint8_t f1 = f_ptr[i + 1];
      uint8_t f2 = f_ptr[i + 2];
      uint8_t f3 = f_ptr[i + 3];
      
      // Branchless accumulation using multiplication by condition
      uint64_t cond0 = (f0 != 0) & (k0 < key_threshold) & ((k0 & 3u) == 0u);
      uint64_t cond1 = (f1 != 0) & (k1 < key_threshold) & ((k1 & 3u) == 0u);
      uint64_t cond2 = (f2 != 0) & (k2 < key_threshold) & ((k2 & 3u) == 0u);
      uint64_t cond3 = (f3 != 0) & (k3 < key_threshold) & ((k3 & 3u) == 0u);
      
      sum += v0 * cond0;
      sum += v1 * cond1;
      sum += v2 * cond2;
      sum += v3 * cond3;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t k = k_ptr[i];
      uint64_t cond = (f_ptr[i] != 0) & (k < key_threshold) & ((k & 3u) == 0u);
      sum += static_cast<uint64_t>(v_ptr[i]) * cond;
    }
  }
  
  return sum;
}