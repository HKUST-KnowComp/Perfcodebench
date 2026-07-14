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
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();
  const size_t starts_size = starts.size();
  const size_t probes_size = probes.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < probes_size; ++i) {
      const uint32_t probe = probes_ptr[i];
      // upper_bound returns the first element > probe
      const uint32_t* it = std::upper_bound(starts_ptr, starts_ptr + starts_size, probe);
      const size_t idx = static_cast<size_t>(std::distance(starts_ptr, it) - 1);
      iter_sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
    total_sum = iter_sum;
  }
  return total_sum;
}