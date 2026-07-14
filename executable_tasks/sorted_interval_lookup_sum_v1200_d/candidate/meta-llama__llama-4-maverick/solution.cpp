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
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < probes.size(); i += 4) {
      const auto it1 = std::upper_bound(starts.begin(), starts.end(), probes[i]);
      const std::size_t idx1 = static_cast<std::size_t>(it1 - starts.begin() - 1);
      sum += static_cast<uint64_t>(payloads[idx1]) * static_cast<uint64_t>(weights[i]);

      if (i + 1 < probes.size()) {
        const auto it2 = std::upper_bound(starts.begin(), starts.end(), probes[i + 1]);
        const std::size_t idx2 = static_cast<std::size_t>(it2 - starts.begin() - 1);
        sum += static_cast<uint64_t>(payloads[idx2]) * static_cast<uint64_t>(weights[i + 1]);
      }

      if (i + 2 < probes.size()) {
        const auto it3 = std::upper_bound(starts.begin(), starts.end(), probes[i + 2]);
        const std::size_t idx3 = static_cast<std::size_t>(it3 - starts.begin() - 1);
        sum += static_cast<uint64_t>(payloads[idx3]) * static_cast<uint64_t>(weights[i + 2]);
      }

      if (i + 3 < probes.size()) {
        const auto it4 = std::upper_bound(starts.begin(), starts.end(), probes[i + 3]);
        const std::size_t idx4 = static_cast<std::size_t>(it4 - starts.begin() - 1);
        sum += static_cast<uint64_t>(payloads[idx4]) * static_cast<uint64_t>(weights[i + 3]);
      }
    }
  }
  return sum;
}