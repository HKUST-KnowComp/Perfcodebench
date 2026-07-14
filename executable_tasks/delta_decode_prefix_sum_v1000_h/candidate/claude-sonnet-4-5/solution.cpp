#include "interface.h"

#include <vector>

uint64_t delta_prefix_sum(const std::vector<uint32_t>& deltas, uint32_t base, int iters) {
  if (iters == 0 || deltas.empty()) {
    return 0;
  }
  
  const std::size_t n = deltas.size();
  
  // Compute the sum for one iteration using a mathematical approach
  // decoded[i] = base + deltas[0] + deltas[1] + ... + deltas[i]
  // sum = sum(decoded[i] for i in 0..n-1)
  //     = sum(base + prefix_sum(deltas, i)) for i in 0..n-1
  //     = n*base + sum(prefix_sum(deltas, i))
  // where prefix_sum(deltas, i) = deltas[0] + ... + deltas[i]
  
  // Each delta[j] appears in (n-j) prefix sums
  // So sum of all prefix sums = sum((n-j) * deltas[j] for j in 0..n-1)
  
  uint64_t delta_contribution = 0;
  for (std::size_t j = 0; j < n; ++j) {
    delta_contribution += (n - j) * static_cast<uint64_t>(deltas[j]);
  }
  
  uint64_t single_iter_sum = n * static_cast<uint64_t>(base) + delta_contribution;
  
  return single_iter_sum * static_cast<uint64_t>(iters);
}
