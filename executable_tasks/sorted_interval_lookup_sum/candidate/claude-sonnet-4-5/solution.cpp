#include "interface.h"

#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();
  const std::size_t n = probes.size();
  const std::size_t starts_size = starts.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    
    std::size_t i = 0;
    const std::size_t n_unroll = n & ~3ULL;
    
    for (; i < n_unroll; i += 4) {
      const uint32_t probe0 = probes_ptr[i];
      const uint32_t probe1 = probes_ptr[i + 1];
      const uint32_t probe2 = probes_ptr[i + 2];
      const uint32_t probe3 = probes_ptr[i + 3];
      
      const uint32_t* it0 = std::lower_bound(starts_ptr, starts_ptr + starts_size, probe0 + 1);
      const uint32_t* it1 = std::lower_bound(starts_ptr, starts_ptr + starts_size, probe1 + 1);
      const uint32_t* it2 = std::lower_bound(starts_ptr, starts_ptr + starts_size, probe2 + 1);
      const uint32_t* it3 = std::lower_bound(starts_ptr, starts_ptr + starts_size, probe3 + 1);
      
      const std::size_t idx0 = it0 - starts_ptr - 1;
      const std::size_t idx1 = it1 - starts_ptr - 1;
      const std::size_t idx2 = it2 - starts_ptr - 1;
      const std::size_t idx3 = it3 - starts_ptr - 1;
      
      sum0 += static_cast<uint64_t>(payloads_ptr[idx0]) * static_cast<uint64_t>(weights_ptr[i]);
      sum1 += static_cast<uint64_t>(payloads_ptr[idx1]) * static_cast<uint64_t>(weights_ptr[i + 1]);
      sum2 += static_cast<uint64_t>(payloads_ptr[idx2]) * static_cast<uint64_t>(weights_ptr[i + 2]);
      sum3 += static_cast<uint64_t>(payloads_ptr[idx3]) * static_cast<uint64_t>(weights_ptr[i + 3]);
    }
    
    for (; i < n; ++i) {
      const uint32_t probe = probes_ptr[i];
      const uint32_t* it = std::lower_bound(starts_ptr, starts_ptr + starts_size, probe + 1);
      const std::size_t idx = it - starts_ptr - 1;
      sum0 += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
    
    sum = sum0 + sum1 + sum2 + sum3;
  }
  
  return sum;
}