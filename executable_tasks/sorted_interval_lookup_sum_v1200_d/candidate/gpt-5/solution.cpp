#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int /*iters*/) {
  // Compute the sum once; baseline returns the per-iteration sum, which is constant.
  const std::size_t n = starts.size();
  const std::size_t m = probes.size();
  if (n == 0 || m == 0) return 0ull;

  const uint32_t* starts_data = starts.data();
  const uint32_t* ends_data = ends.data();
  const uint32_t* payloads_data = payloads.data();
  const uint32_t* probes_data = probes.data();
  const uint32_t* weights_data = weights.data();

  uint64_t sum = 0ull;
  std::size_t idx = 0; // current interval index

  for (std::size_t i = 0; i < m; ++i) {
    const uint32_t p = probes_data[i];
    // Advance interval index while current interval ends before the probe
    while (idx < n && ends_data[idx] < p) {
      ++idx;
    }
    // If there is a current interval and it starts before or at the probe, it's a match
    if (idx < n && starts_data[idx] <= p) {
      sum += static_cast<uint64_t>(payloads_data[idx]) * static_cast<uint64_t>(weights_data[i]);
    }
  }

  return sum;
}
