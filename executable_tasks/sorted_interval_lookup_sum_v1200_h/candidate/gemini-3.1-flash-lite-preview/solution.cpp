#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  uint64_t total_sum = 0;
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();
  const size_t starts_size = starts.size();
  const size_t probes_size = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < probes_size; ++i) {
      uint32_t val = probes_ptr[i];
      int low = 0;
      int high = static_cast<int>(starts_size) - 1;
      int idx = 0;
      while (low <= high) {
        int mid = low + ((high - low) >> 1);
        if (starts_ptr[mid] <= val) {
          idx = mid;
          low = mid + 1;
        } else {
          high = mid - 1;
        }
      }
      current_iter_sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}