#include "interface.h"

#include <cstdint>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = values.size();
  const uint32_t* __restrict v = values.data();
  const uint16_t* __restrict l = lengths.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    std::size_t i = 0;
    
    // Unrolled loop for better ILP
    for (; i + 4 <= n; i += 4) {
      s0 += static_cast<uint64_t>(v[i])     * static_cast<uint64_t>(l[i]);
      s1 += static_cast<uint64_t>(v[i + 1]) * static_cast<uint64_t>(l[i + 1]);
      s2 += static_cast<uint64_t>(v[i + 2]) * static_cast<uint64_t>(l[i + 2]);
      s3 += static_cast<uint64_t>(v[i + 3]) * static_cast<uint64_t>(l[i + 3]);
    }
    
    sum = s0 + s1 + s2 + s3;
    
    // Handle remainder
    for (; i < n; ++i) {
      sum += static_cast<uint64_t>(v[i]) * static_cast<uint64_t>(l[i]);
    }
  }
  return sum;
}
