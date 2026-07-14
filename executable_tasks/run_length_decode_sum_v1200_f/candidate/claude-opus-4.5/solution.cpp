#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_length_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint16_t>& lengths,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = values.size();
  const uint32_t* __restrict v = values.data();
  const uint16_t* __restrict l = lengths.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    // Process 4 elements at a time for better ILP
    for (; i + 4 <= n; i += 4) {
      sum += static_cast<uint64_t>(v[i])     * l[i];
      sum += static_cast<uint64_t>(v[i + 1]) * l[i + 1];
      sum += static_cast<uint64_t>(v[i + 2]) * l[i + 2];
      sum += static_cast<uint64_t>(v[i + 3]) * l[i + 3];
    }
    // Handle remaining elements
    for (; i < n; ++i) {
      sum += static_cast<uint64_t>(v[i]) * l[i];
    }
  }
  return sum;
}