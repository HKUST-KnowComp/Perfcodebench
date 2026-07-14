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
  
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();
  const std::size_t n_starts = starts.size();
  const std::size_t n_probes = probes.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    std::size_t i = 0;
    const std::size_t unroll_limit = n_probes & ~3ULL;
    
    // Unrolled loop processing 4 probes at a time
    for (; i < unroll_limit; i += 4) {
      uint32_t probe0 = probes_ptr[i];
      uint32_t probe1 = probes_ptr[i + 1];
      uint32_t probe2 = probes_ptr[i + 2];
      uint32_t probe3 = probes_ptr[i + 3];
      
      uint32_t weight0 = weights_ptr[i];
      uint32_t weight1 = weights_ptr[i + 1];
      uint32_t weight2 = weights_ptr[i + 2];
      uint32_t weight3 = weights_ptr[i + 3];
      
      // Binary search for probe0
      std::size_t left0 = 0, right0 = n_starts;
      while (left0 < right0) {
        std::size_t mid = left0 + ((right0 - left0) >> 1);
        if (starts_ptr[mid] <= probe0) {
          left0 = mid + 1;
        } else {
          right0 = mid;
        }
      }
      std::size_t idx0 = left0 - 1;
      
      // Binary search for probe1
      std::size_t left1 = 0, right1 = n_starts;
      while (left1 < right1) {
        std::size_t mid = left1 + ((right1 - left1) >> 1);
        if (starts_ptr[mid] <= probe1) {
          left1 = mid + 1;
        } else {
          right1 = mid;
        }
      }
      std::size_t idx1 = left1 - 1;
      
      // Binary search for probe2
      std::size_t left2 = 0, right2 = n_starts;
      while (left2 < right2) {
        std::size_t mid = left2 + ((right2 - left2) >> 1);
        if (starts_ptr[mid] <= probe2) {
          left2 = mid + 1;
        } else {
          right2 = mid;
        }
      }
      std::size_t idx2 = left2 - 1;
      
      // Binary search for probe3
      std::size_t left3 = 0, right3 = n_starts;
      while (left3 < right3) {
        std::size_t mid = left3 + ((right3 - left3) >> 1);
        if (starts_ptr[mid] <= probe3) {
          left3 = mid + 1;
        } else {
          right3 = mid;
        }
      }
      std::size_t idx3 = left3 - 1;
      
      sum += static_cast<uint64_t>(payloads_ptr[idx0]) * weight0;
      sum += static_cast<uint64_t>(payloads_ptr[idx1]) * weight1;
      sum += static_cast<uint64_t>(payloads_ptr[idx2]) * weight2;
      sum += static_cast<uint64_t>(payloads_ptr[idx3]) * weight3;
    }
    
    // Handle remaining probes
    for (; i < n_probes; ++i) {
      uint32_t probe = probes_ptr[i];
      std::size_t left = 0, right = n_starts;
      while (left < right) {
        std::size_t mid = left + ((right - left) >> 1);
        if (starts_ptr[mid] <= probe) {
          left = mid + 1;
        } else {
          right = mid;
        }
      }
      std::size_t idx = left - 1;
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * weights_ptr[i];
    }
  }
  
  return sum;
}