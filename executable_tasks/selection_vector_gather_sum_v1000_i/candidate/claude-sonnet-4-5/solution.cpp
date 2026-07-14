#include "interface.h"

#include <vector>

uint64_t selection_gather_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = keys.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Process in blocks of 4 for better ILP and potential vectorization
    std::size_t i = 0;
    const std::size_t block_end = n - (n % 4);
    
    for (; i < block_end; i += 4) {
      // Unroll 4 iterations
      uint32_t k0 = keys[i];
      uint32_t k1 = keys[i + 1];
      uint32_t k2 = keys[i + 2];
      uint32_t k3 = keys[i + 3];
      
      if (__builtin_expect(flags[i] && k0 < threshold && ((k0 & 7u) == 0u), 0)) {
        sum += payloads[i];
      }
      if (__builtin_expect(flags[i + 1] && k1 < threshold && ((k1 & 7u) == 0u), 0)) {
        sum += payloads[i + 1];
      }
      if (__builtin_expect(flags[i + 2] && k2 < threshold && ((k2 & 7u) == 0u), 0)) {
        sum += payloads[i + 2];
      }
      if (__builtin_expect(flags[i + 3] && k3 < threshold && ((k3 & 7u) == 0u), 0)) {
        sum += payloads[i + 3];
      }
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      uint32_t k = keys[i];
      if (__builtin_expect(flags[i] && k < threshold && ((k & 7u) == 0u), 0)) {
        sum += payloads[i];
      }
    }
  }
  
  return sum;
}