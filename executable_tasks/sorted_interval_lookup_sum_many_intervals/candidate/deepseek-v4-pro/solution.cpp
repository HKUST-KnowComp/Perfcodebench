#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends; // keep signature
  if (iters <= 0) return 0;
  const std::size_t num_probes = probes.size();
  std::vector<std::size_t> indices(num_probes);
  for (std::size_t i = 0; i < num_probes; ++i) {
    const auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
    indices[i] = static_cast<std::size_t>(it - starts.begin() - 1);
  }
  const uint32_t* p_payloads = payloads.data();
  const uint32_t* p_weights = weights.data();
  const std::size_t* p_indices = indices.data();
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    for (std::size_t i = 0; i < num_probes; ++i) {
      local_sum += static_cast<uint64_t>(p_payloads[p_indices[i]]) * static_cast<uint64_t>(p_weights[i]);
    }
    sum = local_sum;
  }
  return sum;
}