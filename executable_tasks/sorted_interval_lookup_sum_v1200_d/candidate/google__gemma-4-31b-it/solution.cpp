#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  const uint32_t* __restrict s_ptr = starts.data();
  const uint32_t* __restrict p_ptr = probes.data();
  const uint32_t* __restrict w_ptr = weights.data();
  const uint32_t* __restrict pay_ptr = payloads.data();
  const size_t n_starts = starts.size();
  const size_t n_probes = probes.size();

  uint64_t total_sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < n_probes; ++i) {
      uint32_t probe = p_ptr[i];
      
      // Manual binary search for upper_bound - 1
      // We want the largest index idx such that starts[idx] <= probe
      size_t low = 0;
      size_t high = n_starts;
      while (low < high) {
        size_t mid = low + (high - low) / 2;
        if (s_ptr[mid] <= probe) {
          low = mid + 1;
        } else {
          high = mid;
        }
      }
      
      // low is now the index of the first element > probe
      // The index we need is low - 1
      if (low > 0) {
        iter_sum += static_cast<uint64_t>(pay_ptr[low - 1]) * static_cast<uint64_t>(w_ptr[i]);
      }
    }
    total_sum = iter_sum;
  }
  
  return total_sum;
}