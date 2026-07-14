#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  (void)iters;
  uint64_t sum = 0;
  const std::size_t n = starts.size();
  const std::size_t m = probes.size();
  if (n == 0 || m == 0) return 0;
  std::size_t i = 0;
  for (std::size_t j = 0; j < m; ++j) {
    uint32_t p = probes[j];
    while (i + 1 < n && starts[i + 1] <= p) {
      ++i;
    }
    sum += static_cast<uint64_t>(payloads[i]) * static_cast<uint64_t>(weights[j]);
  }
  return sum;
}