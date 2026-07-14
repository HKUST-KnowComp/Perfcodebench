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
  const size_t num_intervals = starts.size();
  const size_t num_probes = probes.size();

  // Pre-calculate indices for sorted probes to avoid repeated binary searches
  std::vector<size_t> probe_indices(num_probes);
  size_t current_interval = 0;
  for (size_t i = 0; i < num_probes; ++i) {
    while (current_interval + 1 < num_intervals && starts[current_interval + 1] <= probes[i]) {
      current_interval++;
    }
    probe_indices[i] = current_interval;
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (size_t i = 0; i < num_probes; ++i) {
      iter_sum += static_cast<uint64_t>(payloads[probe_indices[i]]) * static_cast<uint64_t>(weights[i]);
    }
    total_sum = iter_sum;
  }
  return total_sum;
}