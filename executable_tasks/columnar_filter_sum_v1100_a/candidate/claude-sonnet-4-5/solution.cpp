#include "interface.h"

#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* __restrict__ k_ptr = keys.data();
  const uint32_t* __restrict__ v_ptr = values.data();
  const uint8_t* __restrict__ f_ptr = flags.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    std::size_t i = 0;
    
    // Main vectorizable loop - process 4 elements at a time
    for (; i + 3 < n; i += 4) {
      uint32_t k0 = k_ptr[i];
      uint32_t k1 = k_ptr[i + 1];
      uint32_t k2 = k_ptr[i + 2];
      uint32_t k3 = k_ptr[i + 3];
      
      uint8_t f0 = f_ptr[i];
      uint8_t f1 = f_ptr[i + 1];
      uint8_t f2 = f_ptr[i + 2];
      uint8_t f3 = f_ptr[i + 3];
      
      // Branchless condition evaluation
      bool cond0 = f0 && (k0 < key_threshold) && ((k0 & 3u) == 0u);
      bool cond1 = f1 && (k1 < key_threshold) && ((k1 & 3u) == 0u);
      bool cond2 = f2 && (k2 < key_threshold) && ((k2 & 3u) == 0u);
      bool cond3 = f3 && (k3 < key_threshold) && ((k3 & 3u) == 0u);
      
      // Conditional accumulation using multiplication by boolean
      sum += static_cast<uint64_t>(v_ptr[i]) * cond0;
      sum += static_cast<uint64_t>(v_ptr[i + 1]) * cond1;
      sum += static_cast<uint64_t>(v_ptr[i + 2]) * cond2;
      sum += static_cast<uint64_t>(v_ptr[i + 3]) * cond3;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      if (f_ptr[i] && k_ptr[i] < key_threshold && ((k_ptr[i] & 3u) == 0u)) {
        sum += static_cast<uint64_t>(v_ptr[i]);
      }
    }
  }
  
  return sum;
}