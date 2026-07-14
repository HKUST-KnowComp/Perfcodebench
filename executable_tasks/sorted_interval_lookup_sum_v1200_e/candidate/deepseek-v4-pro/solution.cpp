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
  (void)ends;
  if (iters <= 0) return 0;
  
  const std::size_t num_probes = probes.size();
  std::vector<std::size_t> idx(num_probes);
  for (std::size_t i = 0; i < num_probes; ++i) {
    const auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
    idx[i] = static_cast<std::size_t>(it - starts.begin() - 1);
  }
  
  uint64_t single_sum = 0;
  for (std::size_t i = 0; i < num_probes; ++i) {
    single_sum += static_cast<uint64_t>(payloads[idx[i]]) * static_cast<uint64_t>(weights[i]);
  }
  
  return single_sum * static_cast<uint64_t>(iters);
}