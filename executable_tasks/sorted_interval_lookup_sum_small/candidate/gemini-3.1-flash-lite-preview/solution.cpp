#include "interface.h"

#include <vector>
#include <cstdint>

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
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < m; ++i) {
      uint32_t probe = probes_ptr[i];
      int low = 0;
      int high = static_cast<int>(n) - 1;
      int idx = 0;
      while (low <= high) {
        int mid = low + ((high - low) >> 1);
        if (starts_ptr[mid] <= probe) {
          idx = mid;
          low = mid + 1;
        } else {
          high = mid - 1;
        }
      }
      iter_sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
    total_sum = iter_sum;
  }
  return total_sum;
}