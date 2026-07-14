#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  uint64_t total_sum = 0;
  const size_t num_starts = starts.size();
  const size_t num_probes = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t last_idx = 0;
    
    for (size_t i = 0; i < num_probes; ++i) {
      uint32_t p = probes[i];
      
      // Since probes are sorted, we can start searching from the previous index
      // Use binary search on the remaining range [last_idx, num_starts)
      auto it = std::upper_bound(starts.begin() + last_idx, starts.end(), p);
      size_t idx = static_cast<size_t>(it - starts.begin() - 1);
      
      current_sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
      last_idx = idx;
    }
    total_sum = current_sum;
  }
  return total_sum;
}