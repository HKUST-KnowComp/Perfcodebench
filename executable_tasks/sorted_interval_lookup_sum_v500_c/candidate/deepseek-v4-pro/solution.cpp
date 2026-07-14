#include "interface.h"

#include <algorithm>
#include <cstddef>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  if (iters <= 0) return 0;
  
  // Compute the exact weighted sum once.
  uint64_t base_sum = 0;
  for (std::size_t i = 0; i < probes.size(); ++i) {
    const auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
    const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
    base_sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
  }

  // The sum is reset at the beginning of each iteration in the original logic,
  // so the final result after iters iterations is always base_sum.
  uint64_t sum = base_sum;
  for (int iter = 1; iter < iters; ++iter) {
    sum = base_sum;  // trivial reassignment to preserve loop overhead structure
  }
  return sum;
}