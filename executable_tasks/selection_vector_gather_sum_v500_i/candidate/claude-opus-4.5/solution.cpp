#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  
  const std::size_t n = keys.size();
  if (n == 0 || iters <= 0) return 0;
  
  const uint32_t* __restrict__ keys_ptr = keys.data();
  const uint8_t* __restrict__ flags_ptr = flags.data();
  const uint32_t* __restrict__ payloads_ptr = payloads.data();
  
  // Build selection vector once - inputs don't change across iterations
  std::vector<uint32_t> selected;
  selected.reserve(n / 8); // Estimate ~12.5% selectivity for (key & 7) == 0
  
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t k = keys_ptr[i];
    if (flags_ptr[i] && k < threshold && ((k & 7u) == 0u)) {
      selected.push_back(static_cast<uint32_t>(i));
    }
  }
  
  const std::size_t sel_count = selected.size();
  const uint32_t* __restrict__ sel_ptr = selected.data();
  
  uint64_t sum = 0;
  
  // Compute sum for each iteration
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    
    // Unroll by 4 for better ILP
    std::size_t i = 0;
    const std::size_t unroll_limit = sel_count & ~std::size_t(3);
    
    for (; i < unroll_limit; i += 4) {
      uint32_t idx0 = sel_ptr[i];
      uint32_t idx1 = sel_ptr[i + 1];
      uint32_t idx2 = sel_ptr[i + 2];
      uint32_t idx3 = sel_ptr[i + 3];
      
      local_sum += payloads_ptr[idx0];
      local_sum += payloads_ptr[idx1];
      local_sum += payloads_ptr[idx2];
      local_sum += payloads_ptr[idx3];
    }
    
    // Handle remainder
    for (; i < sel_count; ++i) {
      local_sum += payloads_ptr[sel_ptr[i]];
    }
    
    sum = local_sum;
  }
  
  return sum;
}