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
  const size_t n = starts.size();
  const size_t m = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_sum = 0;
    size_t low = 0;
    for (size_t i = 0; i < m; ++i) {
      uint32_t probe = probes[i];
      size_t high = n;
      size_t current_low = low;
      
      // Binary search for the interval index
      while (current_low < high) {
        size_t mid = current_low + ((high - current_low) >> 1);
        if (starts[mid] <= probe) {
          current_low = mid + 1;
        } else {
          high = mid;
        }
      }
      
      size_t idx = current_low - 1;
      current_sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
      
      // Optimization: next probe is >= current probe, so search range starts at idx
      low = idx;
    }
    total_sum = current_sum;
  }
  return total_sum;
}