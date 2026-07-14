#include "interface.h"
#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  const uint32_t* s_ptr = starts.data();
  const uint32_t* p_ptr = payloads.data();
  const uint32_t* pr_ptr = probes.data();
  const uint32_t* w_ptr = weights.data();
  const size_t s_size = starts.size();
  const size_t pr_size = probes.size();

  uint64_t total_sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < pr_size; ++i) {
      uint32_t probe = pr_ptr[i];
      
      // Custom binary search for upper_bound - 1
      int low = 0;
      int high = static_cast<int>(s_size) - 1;
      int idx = 0;
      while (low <= high) {
        int mid = low + ((high - low) >> 1);
        if (s_ptr[mid] <= probe) {
          idx = mid;
          low = mid + 1;
        } else {
          high = mid - 1;
        }
      }
      current_iter_sum += static_cast<uint64_t>(p_ptr[idx]) * static_cast<uint64_t>(w_ptr[i]);
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}