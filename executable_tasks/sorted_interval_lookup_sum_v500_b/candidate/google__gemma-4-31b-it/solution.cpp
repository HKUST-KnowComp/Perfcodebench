#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  const uint32_t* s_ptr = starts.data();
  const uint32_t* p_ptr = probes.data();
  const uint32_t* w_ptr = weights.data();
  const uint32_t* pay_ptr = payloads.data();
  const size_t n_starts = starts.size();
  const size_t n_probes = probes.size();

  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t low = 0;
    for (size_t i = 0; i < n_probes; ++i) {
      uint32_t probe = p_ptr[i];
      size_t high = n_starts;
      
      // Binary search for the largest index idx such that starts[idx] <= probe
      // Since probes are sorted, we can start the search from the previous 'low'
      size_t left = low;
      size_t right = high;
      while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (s_ptr[mid] <= probe) {
          left = mid + 1;
        } else {
          right = mid;
        }
      }
      
      // The index we want is left - 1
      size_t idx = left - 1;
      current_sum += static_cast<uint64_t>(pay_ptr[idx]) * static_cast<uint64_t>(w_ptr[i]);
      
      // Update low for the next probe to narrow the search range
      low = idx;
    }
    total_sum = current_sum;
  }
  
  return total_sum;
}