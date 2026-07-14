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
  selected.reserve(n / 8); // Estimate based on (keys[i] & 7) == 0 filter
  
  std::size_t i = 0;
  const std::size_t n4 = n & ~std::size_t(3);
  
  // Unrolled selection loop
  for (; i < n4; i += 4) {
    uint32_t k0 = keys_ptr[i];
    uint32_t k1 = keys_ptr[i + 1];
    uint32_t k2 = keys_ptr[i + 2];
    uint32_t k3 = keys_ptr[i + 3];
    
    if (flags_ptr[i] && k0 < threshold && ((k0 & 7u) == 0u)) {
      selected.push_back(static_cast<uint32_t>(i));
    }
    if (flags_ptr[i + 1] && k1 < threshold && ((k1 & 7u) == 0u)) {
      selected.push_back(static_cast<uint32_t>(i + 1));
    }
    if (flags_ptr[i + 2] && k2 < threshold && ((k2 & 7u) == 0u)) {
      selected.push_back(static_cast<uint32_t>(i + 2));
    }
    if (flags_ptr[i + 3] && k3 < threshold && ((k3 & 7u) == 0u)) {
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
  
  // Compute sum once and multiply by iters? No - must iterate for timing.
  // But we can optimize the gather loop.
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    std::size_t j = 0;
    const std::size_t sel4 = sel_size & ~std::size_t(3);
    
    // Unrolled gather-sum loop with multiple accumulators to hide latency
    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    
    for (; j < sel4; j += 4) {
      sum0 += payloads_ptr[sel_ptr[j]];
      sum1 += payloads_ptr[sel_ptr[j + 1]];
      sum2 += payloads_ptr[sel_ptr[j + 2]];
      sum3 += payloads_ptr[sel_ptr[j + 3]];
    }
    
    local_sum = sum0 + sum1 + sum2 + sum3;
    
    // Handle remainder
    for (; j < sel_size; ++j) {
      local_sum += payloads_ptr[sel_ptr[j]];
    }
    
    sum = local_sum;
  }
  
  return sum;
}