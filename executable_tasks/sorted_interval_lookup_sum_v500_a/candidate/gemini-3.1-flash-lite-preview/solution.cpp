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
  const size_t num_probes = probes.size();
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();
  const size_t starts_size = starts.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t current_iter_sum = 0;
    for (size_t i = 0; i < num_probes; ++i) {
      const uint32_t probe = probes_ptr[i];
      // upper_bound finds the first element > probe
      auto it = std::upper_bound(starts_ptr, starts_ptr + starts_size, probe);
      size_t idx = static_cast<size_t>(std::distance(starts_ptr, it) - 1);
      current_iter_sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
    total_sum = current_iter_sum;
  }
  return total_sum;
}