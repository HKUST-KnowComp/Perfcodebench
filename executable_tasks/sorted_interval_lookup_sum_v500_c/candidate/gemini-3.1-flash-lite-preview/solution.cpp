#include "interface.h"
#include <algorithm>
#include <iterator>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  uint64_t total_sum = 0;
  const size_t n_starts = starts.size();
  const size_t n_probes = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    auto it = starts.begin();
    
    for (size_t i = 0; i < n_probes; ++i) {
      // Since probes are sorted, we can use the previous search position as a hint
      // std::upper_bound is efficient, but we can optimize by narrowing the range
      it = std::upper_bound(it, starts.end(), probes[i]);
      
      if (it != starts.begin()) {
        size_t idx = std::distance(starts.begin(), std::prev(it));
        current_iter_sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
      }
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}