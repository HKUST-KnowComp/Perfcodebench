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
  
  const size_t n_intervals = starts.size();
  const size_t n_probes = probes.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    
    size_t i = 0;
    const size_t n_unrolled = n_probes & ~size_t(3);
    
    for (; i < n_unrolled; i += 4) {
      uint32_t p0 = probes_ptr[i];
      uint32_t p1 = probes_ptr[i + 1];
      uint32_t p2 = probes_ptr[i + 2];
      uint32_t p3 = probes_ptr[i + 3];
      
      // Manual binary search for upper_bound - 1
      size_t lo0 = 0, hi0 = n_intervals;
      size_t lo1 = 0, hi1 = n_intervals;
      size_t lo2 = 0, hi2 = n_intervals;
      size_t lo3 = 0, hi3 = n_intervals;
      
      while (lo0 < hi0) {
        size_t mid = lo0 + ((hi0 - lo0) >> 1);
        if (starts_ptr[mid] <= p0) lo0 = mid + 1;
        else hi0 = mid;
      }
      
      while (lo1 < hi1) {
        size_t mid = lo1 + ((hi1 - lo1) >> 1);
        if (starts_ptr[mid] <= p1) lo1 = mid + 1;
        else hi1 = mid;
      }
      
      while (lo2 < hi2) {
        size_t mid = lo2 + ((hi2 - lo2) >> 1);
        if (starts_ptr[mid] <= p2) lo2 = mid + 1;
        else hi2 = mid;
      }
      
      while (lo3 < hi3) {
        size_t mid = lo3 + ((hi3 - lo3) >> 1);
        if (starts_ptr[mid] <= p3) lo3 = mid + 1;
        else hi3 = mid;
      }
      
      size_t idx0 = lo0 - 1;
      size_t idx1 = lo1 - 1;
      size_t idx2 = lo2 - 1;
      size_t idx3 = lo3 - 1;
      
      sum0 += static_cast<uint64_t>(payloads_ptr[idx0]) * static_cast<uint64_t>(weights_ptr[i]);
      sum1 += static_cast<uint64_t>(payloads_ptr[idx1]) * static_cast<uint64_t>(weights_ptr[i + 1]);
      sum2 += static_cast<uint64_t>(payloads_ptr[idx2]) * static_cast<uint64_t>(weights_ptr[i + 2]);
      sum3 += static_cast<uint64_t>(payloads_ptr[idx3]) * static_cast<uint64_t>(weights_ptr[i + 3]);
    }
    
    // Handle remaining elements
    for (; i < n_probes; ++i) {
      uint32_t p = probes_ptr[i];
      size_t lo = 0, hi = n_intervals;
      while (lo < hi) {
        size_t mid = lo + ((hi - lo) >> 1);
        if (starts_ptr[mid] <= p) lo = mid + 1;
        else hi = mid;
      }
      size_t idx = lo - 1;
      sum0 += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
    
    sum = sum0 + sum1 + sum2 + sum3;
  }
  
  return sum;
}