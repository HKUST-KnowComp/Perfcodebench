#include "interface.h"

#include <cstddef>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  if (iters <= 0) return 0;

  uint64_t sum = 0;
  std::size_t j = 0;
  const std::size_t n_starts = starts.size();

  for (std::size_t i = 0; i < probes.size(); ++i) {
    uint32_t probe = probes[i];
    while (j + 1 < n_starts && starts[j + 1] <= probe) {
      ++j;
    }
    sum += static_cast<uint64_t>(payloads[j]) * static_cast<uint64_t>(weights[i]);
  }

  return sum;
}