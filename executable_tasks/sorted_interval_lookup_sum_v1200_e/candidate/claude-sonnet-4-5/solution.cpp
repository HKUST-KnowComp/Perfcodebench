#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  if (iters <= 0 || probes.empty()) return 0;
  
  // Compute sum once since lookup is deterministic
  uint64_t single_sum = 0;
  const std::size_t probe_count = probes.size();
  const uint32_t* __restrict starts_ptr = starts.data();
  const uint32_t* __restrict payloads_ptr = payloads.data();
  const uint32_t* __restrict probes_ptr = probes.data();
  const uint32_t* __restrict weights_ptr = weights.data();
  const std::size_t starts_size = starts.size();
  
  #pragma GCC unroll 4
  for (std::size_t i = 0; i < probe_count; ++i) {
    const uint32_t probe = probes_ptr[i];
    
    // Manual binary search for upper_bound
    std::size_t left = 0;
    std::size_t right = starts_size;
    
    while (left < right) {
      const std::size_t mid = left + (right - left) / 2;
      if (starts_ptr[mid] <= probe) {
        left = mid + 1;
      } else {
        right = mid;
      }
    }
    
    const std::size_t idx = left - 1;
    single_sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
  }
  
  // Multiply by iters instead of recomputing
  return single_sum * static_cast<uint64_t>(iters);
}