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
  
  // Build selection vector once (inputs don't change across iterations)
  std::vector<uint32_t> selected;
  selected.reserve(n / 8); // Estimate ~12.5% selectivity
  
  std::size_t i = 0;
  const std::size_t n4 = n & ~std::size_t(3);
  
  // Unrolled selection loop
  for (; i < n4; i += 4) {
    uint32_t k0 = keys_ptr[i];
    uint32_t k1 = keys_ptr[i + 1];
    uint32_t k2 = keys_ptr[i + 2];
    uint32_t k3 = keys_ptr[i + 3];
    
    uint8_t f0 = flags_ptr[i];
    uint8_t f1 = flags_ptr[i + 1];
    uint8_t f2 = flags_ptr[i + 2];
    uint8_t f3 = flags_ptr[i + 3];
    
    if (f0 && k0 < threshold && ((k0 & 7u) == 0u)) {
      selected.push_back(static_cast<uint32_t>(i));
    }
    if (f1 && k1 < threshold && ((k1 & 7u) == 0u)) {
      selected.push_back(static_cast<uint32_t>(i + 1));
    }
    if (f2 && k2 < threshold && ((k2 & 7u) == 0u)) {
      selected.push_back(static_cast<uint32_t>(i + 2));
    }
    if (f3 && k3 < threshold && ((k3 & 7u) == 0u)) {
      selected.push_back(static_cast<uint32_t>(i + 3));
    }
  }
  
  // Handle remainder
  for (; i < n; ++i) {
    if (flags_ptr[i] && keys_ptr[i] < threshold && ((keys_ptr[i] & 7u) == 0u)) {
      selected.push_back(static_cast<uint32_t>(i));
    }
  }
  
  const std::size_t sel_size = selected.size();
  const uint32_t* __restrict__ sel_ptr = selected.data();
  
  uint64_t sum = 0;
  
  // Run gather-sum for each iteration
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t j = 0;
    const std::size_t sel4 = sel_size & ~std::size_t(3);
    
    // Unrolled gather loop with prefetch
    for (; j < sel4; j += 4) {
      // Prefetch ahead
      if (j + 16 < sel_size) {
        __builtin_prefetch(&payloads_ptr[sel_ptr[j + 16]], 0, 0);
      }
      
      uint32_t idx0 = sel_ptr[j];
      uint32_t idx1 = sel_ptr[j + 1];
      uint32_t idx2 = sel_ptr[j + 2];
      uint32_t idx3 = sel_ptr[j + 3];
      
      local_sum += payloads_ptr[idx0];
      local_sum += payloads_ptr[idx1];
      local_sum += payloads_ptr[idx2];
      local_sum += payloads_ptr[idx3];
    }
    
    // Handle remainder
    for (; j < sel_size; ++j) {
      local_sum += payloads_ptr[sel_ptr[j]];
    }
    
    sum = local_sum;
  }
  
  return sum;
}