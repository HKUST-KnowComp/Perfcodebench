#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  const std::size_t n = keys.size();
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    
    // Main unrolled loop
    for (; i + 3 < n; i += 4) {
      // Process 4 elements at once
      if (__builtin_expect(flags[i] && keys[i] < threshold && ((keys[i] & 7u) == 0u), 0)) {
        sum += static_cast<uint64_t>(payloads[i]);
      }
      if (__builtin_expect(flags[i+1] && keys[i+1] < threshold && ((keys[i+1] & 7u) == 0u), 0)) {
        sum += static_cast<uint64_t>(payloads[i+1]);
      }
      if (__builtin_expect(flags[i+2] && keys[i+2] < threshold && ((keys[i+2] & 7u) == 0u), 0)) {
        sum += static_cast<uint64_t>(payloads[i+2]);
      }
      if (__builtin_expect(flags[i+3] && keys[i+3] < threshold && ((keys[i+3] & 7u) == 0u), 0)) {
        sum += static_cast<uint64_t>(payloads[i+3]);
      }
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      if (__builtin_expect(flags[i] && keys[i] < threshold && ((keys[i] & 7u) == 0u), 0)) {
        sum += static_cast<uint64_t>(payloads[i]);
      }
    }
  }
  
  return sum;
}