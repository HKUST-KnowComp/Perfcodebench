#include "interface.h"
#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  uint64_t sum = 0;
  const size_t n = starts.size();
  const size_t m = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t last_idx = 0;
    for (size_t i = 0; i < m; ++i) {
      uint32_t p = probes[i];
      
      // Use the previous index as a hint for the search range
      size_t low = last_idx;
      size_t high = n;
      
      // Narrow search range if possible
      if (p < starts[low]) {
          high = low;
          low = 0;
      }

      // Binary search within [low, high)
      size_t count = high - low;
      while (count > 0) {
        size_t step = count / 2;
        size_t mid = low + step;
        if (!(p < starts[mid])) {
          low = mid + 1;
          count -= step + 1;
        } else {
          count = step;
        }
      }
      
      last_idx = low - 1;
      sum += static_cast<uint64_t>(payloads[last_idx]) * static_cast<uint64_t>(weights[i]);
    }
  }
  return sum;
}