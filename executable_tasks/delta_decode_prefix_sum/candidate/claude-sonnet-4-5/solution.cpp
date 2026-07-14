#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  const std::size_t n = deltas.size();
  
  // Precompute the sum of deltas and the final decoded value once
  uint64_t delta_sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    delta_sum += deltas[i];
  }
  
  // The prefix sum can be computed as:
  // sum = n*base + (n-1)*deltas[0] + (n-2)*deltas[1] + ... + 1*deltas[n-2] + 0*deltas[n-1]
  // Which is: n*base + sum_{i=0}^{n-1}((n-1-i)*deltas[i])
  uint64_t weighted_sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    weighted_sum += (n - 1 - i) * static_cast<uint64_t>(deltas[i]);
  }
  
  uint64_t single_iter_sum = n * static_cast<uint64_t>(base) + weighted_sum;
  
  return single_iter_sum * iters;
}