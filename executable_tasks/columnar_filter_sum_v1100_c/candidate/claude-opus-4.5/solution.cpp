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
  const uint32_t* __restrict k_ptr = keys.data();
  const uint32_t* __restrict v_ptr = values.data();
  const uint8_t* __restrict f_ptr = flags.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    
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
      
      uint32_t f0 = f_ptr[i];
      uint32_t f1 = f_ptr[i + 1];
      uint32_t f2 = f_ptr[i + 2];
      uint32_t f3 = f_ptr[i + 3];
      
      // Branchless predicate: flags[i] && keys[i] < key_threshold && ((keys[i] & 3u) == 0u)
      // Convert each condition to 0 or 1, multiply together, then use as mask
      uint32_t cond0 = (f0 != 0) & (k0 < key_threshold) & ((k0 & 3u) == 0u);
      uint32_t cond1 = (f1 != 0) & (k1 < key_threshold) & ((k1 & 3u) == 0u);
      uint32_t cond2 = (f2 != 0) & (k2 < key_threshold) & ((k2 & 3u) == 0u);
      uint32_t cond3 = (f3 != 0) & (k3 < key_threshold) & ((k3 & 3u) == 0u);
      
      // Branchless accumulation
      local_sum += static_cast<uint64_t>(v0) * cond0;
      local_sum += static_cast<uint64_t>(v1) * cond1;
      local_sum += static_cast<uint64_t>(v2) * cond2;
      local_sum += static_cast<uint64_t>(v3) * cond3;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t k = k_ptr[i];
      uint32_t cond = (f_ptr[i] != 0) & (k < key_threshold) & ((k & 3u) == 0u);
      local_sum += static_cast<uint64_t>(v_ptr[i]) * cond;
    }
    
    sum = local_sum;
  }
  
  return sum;
}