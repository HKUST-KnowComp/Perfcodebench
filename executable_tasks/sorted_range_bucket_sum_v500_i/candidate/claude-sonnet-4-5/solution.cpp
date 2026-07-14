#include "interface.h"

#include <algorithm>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  uint64_t sum = 0;
  const size_t b_size = boundaries.size();
  const size_t p_size = probes.size();
  
  if (b_size == 0 || p_size == 0) {
    return 0;
  }
  
  const uint32_t* b_data = boundaries.data();
  const uint32_t* p_data = probes.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t last_pos = 0;
    
    for (size_t i = 0; i < p_size; ++i) {
      uint32_t probe = p_data[i];
      
      // Galloping search: exploit sorted probes by starting from last position
      size_t left = last_pos;
      size_t right = b_size;
      
      // Quick check if we're still in the same bucket
      if (left < b_size && b_data[left] > probe) {
        // Probe decreased, need to search backwards (rare for sorted data)
        left = 0;
      } else if (left < b_size) {
        // Gallop forward to find the range
        size_t step = 1;
        while (left + step < b_size && b_data[left + step] <= probe) {
          left += step;
          step <<= 1;
        }
        right = std::min(left + step + 1, b_size);
      }
      
      // Binary search in the reduced range
      while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (b_data[mid] <= probe) {
          left = mid + 1;
        } else {
          right = mid;
        }
      }
      
      sum += left;
      last_pos = left;
    }
  }
  
  return sum;
}