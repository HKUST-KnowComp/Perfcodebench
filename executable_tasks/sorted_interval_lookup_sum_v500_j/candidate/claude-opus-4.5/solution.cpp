#include "interface.h"

#include <cstdint>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  const uint32_t* __restrict__ starts_ptr = starts.data();
  const uint32_t* __restrict__ payloads_ptr = payloads.data();
  const uint32_t* __restrict__ probes_ptr = probes.data();
  const uint32_t* __restrict__ weights_ptr = weights.data();
  
  const std::size_t n_intervals = starts.size();
  const std::size_t n_probes = probes.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    
    std::size_t i = 0;
    const std::size_t n_unrolled = n_probes & ~std::size_t(3);
    
    for (; i < n_unrolled; i += 4) {
      const uint32_t p0 = probes_ptr[i];
      const uint32_t p1 = probes_ptr[i + 1];
      const uint32_t p2 = probes_ptr[i + 2];
      const uint32_t p3 = probes_ptr[i + 3];
      
      // Binary search for p0
      std::size_t lo0 = 0, hi0 = n_intervals;
      while (lo0 < hi0) {
        std::size_t mid = lo0 + ((hi0 - lo0) >> 1);
        if (starts_ptr[mid] <= p0) lo0 = mid + 1;
        else hi0 = mid;
      }
      
      // Binary search for p1
      std::size_t lo1 = 0, hi1 = n_intervals;
      while (lo1 < hi1) {
        std::size_t mid = lo1 + ((hi1 - lo1) >> 1);
        if (starts_ptr[mid] <= p1) lo1 = mid + 1;
        else hi1 = mid;
      }
      
      // Binary search for p2
      std::size_t lo2 = 0, hi2 = n_intervals;
      while (lo2 < hi2) {
        std::size_t mid = lo2 + ((hi2 - lo2) >> 1);
        if (starts_ptr[mid] <= p2) lo2 = mid + 1;
        else hi2 = mid;
      }
      
      // Binary search for p3
      std::size_t lo3 = 0, hi3 = n_intervals;
      while (lo3 < hi3) {
        std::size_t mid = lo3 + ((hi3 - lo3) >> 1);
        if (starts_ptr[mid] <= p3) lo3 = mid + 1;
        else hi3 = mid;
      }
      
      local_sum += static_cast<uint64_t>(payloads_ptr[lo0 - 1]) * static_cast<uint64_t>(weights_ptr[i]);
      local_sum += static_cast<uint64_t>(payloads_ptr[lo1 - 1]) * static_cast<uint64_t>(weights_ptr[i + 1]);
      local_sum += static_cast<uint64_t>(payloads_ptr[lo2 - 1]) * static_cast<uint64_t>(weights_ptr[i + 2]);
      local_sum += static_cast<uint64_t>(payloads_ptr[lo3 - 1]) * static_cast<uint64_t>(weights_ptr[i + 3]);
    }
    
    for (; i < n_probes; ++i) {
      const uint32_t p = probes_ptr[i];
      std::size_t lo = 0, hi = n_intervals;
      while (lo < hi) {
        std::size_t mid = lo + ((hi - lo) >> 1);
        if (starts_ptr[mid] <= p) lo = mid + 1;
        else hi = mid;
      }
      local_sum += static_cast<uint64_t>(payloads_ptr[lo - 1]) * static_cast<uint64_t>(weights_ptr[i]);
    }
    
    sum = local_sum;
  }
  
  return sum;
}