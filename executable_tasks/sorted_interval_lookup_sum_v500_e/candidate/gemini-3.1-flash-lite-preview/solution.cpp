#include "interface.h"
#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  uint64_t total_sum = 0;
  const uint32_t* s_ptr = starts.data();
  const uint32_t* p_ptr = payloads.data();
  const uint32_t* pr_ptr = probes.data();
  const uint32_t* w_ptr = weights.data();
  const size_t n = starts.size();
  const size_t m = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    for (size_t i = 0; i < m; ++i) {
      uint32_t val = pr_ptr[i];
      size_t low = 0;
      size_t high = n;
      
      // Binary search for the largest index such that starts[idx] <= val
      while (low < high) {
        size_t mid = low + ((high - low) >> 1);
        if (s_ptr[mid] <= val) {
          low = mid + 1;
        } else {
          high = mid;
        }
      }
      
      if (low > 0) {
        current_sum += static_cast<uint64_t>(p_ptr[low - 1]) * static_cast<uint64_t>(w_ptr[i]);
      }
    }
    total_sum = current_sum;
  }
  return total_sum;
}