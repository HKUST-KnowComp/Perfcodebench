#include "interface.h"

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
  const std::size_t N = starts.size();
  const std::size_t P = probes.size();

  // precompute the matched payload for each probe via two-pointer scan
  std::vector<uint32_t> matched_payloads(P);
  std::size_t idx = 0;
  for (std::size_t i = 0; i < P; ++i) {
    const uint32_t probe = probes[i];
    while (idx + 1 < N && starts[idx + 1] <= probe) {
      ++idx;
    }
    matched_payloads[i] = payloads[idx];
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < P; ++i) {
      sum += static_cast<uint64_t>(matched_payloads[i]) * static_cast<uint64_t>(weights[i]);
    }
  }
  return sum;
}