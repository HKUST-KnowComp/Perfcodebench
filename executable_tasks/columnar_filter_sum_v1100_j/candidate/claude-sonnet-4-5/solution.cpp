#include "interface.h"

#include <vector>

uint64_t filter_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint8_t>& flags,
    uint32_t key_threshold,
    int iters) {
  uint64_t sum = 0;
  const std::size_t size = keys.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Process in chunks of 4 for better ILP
    std::size_t i = 0;
    const std::size_t limit = size - (size % 4);
    
    for (; i < limit; i += 4) {
      // Unroll 4 iterations
      uint64_t local_sum = 0;
      
      if (flags[i] && keys[i] < key_threshold && ((keys[i] & 3u) == 0u)) {
        local_sum += values[i];
      }
      if (flags[i+1] && keys[i+1] < key_threshold && ((keys[i+1] & 3u) == 0u)) {
        local_sum += values[i+1];
      }
      if (flags[i+2] && keys[i+2] < key_threshold && ((keys[i+2] & 3u) == 0u)) {
        local_sum += values[i+2];
      }
      if (flags[i+3] && keys[i+3] < key_threshold && ((keys[i+3] & 3u) == 0u)) {
        local_sum += values[i+3];
      }
      
      sum += local_sum;
    }
    
    // Handle remainder
    for (; i < size; ++i) {
      if (flags[i] && keys[i] < key_threshold && ((keys[i] & 3u) == 0u)) {
        sum += values[i];
      }
    }
  }
  
  return sum;
}