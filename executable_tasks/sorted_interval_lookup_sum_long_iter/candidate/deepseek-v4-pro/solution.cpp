#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)iters;
  (void)ends;  // ends not used in baseline lookup
  const size_t N = starts.size();
  const size_t M = probes.size();
  size_t i = 0;
  uint64_t total_sum = 0;
  for (size_t j = 0; j < M; ++j) {
    uint32_t p = probes[j];
    // advance i to the last index with starts[i] <= p
    while (i + 1 < N && starts[i + 1] <= p) {
      ++i;
    }
    total_sum += static_cast<uint64_t>(payloads[i]) * static_cast<uint64_t>(weights[j]);
  }
  return total_sum;
}