#include "interface.h"
#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  const size_t n = starts.size();
  const size_t m = probes.size();
  const uint32_t* s_ptr = starts.data();
  const uint32_t* p_ptr = payloads.data();
  const uint32_t* pr_ptr = probes.data();
  const uint32_t* w_ptr = weights.data();

  uint64_t total_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t last_idx = 0;
    for (size_t i = 0; i < m; ++i) {
      uint32_t probe = pr_ptr[i];
      
      // Binary search range optimization: since probes are sorted,
      // the target interval index is non-decreasing.
      size_t low = last_idx;
      size_t high = n;
      while (low < high) {
        size_t mid = low + ((high - low) >> 1);
        if (s_ptr[mid] <= probe) {
          low = mid + 1;
        } else {
          high = mid;
        }
      }
      last_idx = low - 1;
      current_sum += static_cast<uint64_t>(p_ptr[last_idx]) * static_cast<uint64_t>(w_ptr[i]);
    }
    total_sum = current_sum;
  }
  return total_sum;
}