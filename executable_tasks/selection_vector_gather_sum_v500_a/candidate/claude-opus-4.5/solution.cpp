#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const std::size_t n = keys.size();
  const uint32_t* __restrict__ k = keys.data();
  const uint8_t* __restrict__ f = flags.data();
  const uint32_t* __restrict__ p = payloads.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t i = 0;
    
    // Process 4 elements at a time for better ILP
    const std::size_t n4 = n & ~std::size_t(3);
    for (; i < n4; i += 4) {
      uint32_t k0 = k[i];
      uint32_t k1 = k[i + 1];
      uint32_t k2 = k[i + 2];
      uint32_t k3 = k[i + 3];
      
      // Branchless predicate evaluation
      bool c0 = f[i] && (k0 < threshold) && ((k0 & 7u) == 0u);
      bool c1 = f[i + 1] && (k1 < threshold) && ((k1 & 7u) == 0u);
      bool c2 = f[i + 2] && (k2 < threshold) && ((k2 & 7u) == 0u);
      bool c3 = f[i + 3] && (k3 < threshold) && ((k3 & 7u) == 0u);
      
      // Conditional add using multiplication by boolean (compiles to cmov or branchless)
      local_sum += static_cast<uint64_t>(p[i]) * c0;
      local_sum += static_cast<uint64_t>(p[i + 1]) * c1;
      local_sum += static_cast<uint64_t>(p[i + 2]) * c2;
      local_sum += static_cast<uint64_t>(p[i + 3]) * c3;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t ki = k[i];
      bool cond = f[i] && (ki < threshold) && ((ki & 7u) == 0u);
      local_sum += static_cast<uint64_t>(p[i]) * cond;
    }
    
    sum = local_sum;
  }
  
  return sum;
}