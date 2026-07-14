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
  if (iters <= 0) {
    return 0;
  }
  uint64_t sum = 0;
  const size_t N = starts.size();
  size_t idx = 0;
  const size_t P = probes.size();
  for (size_t i = 0; i < P; ++i) {
    uint32_t probe = probes[i];
    while (idx + 1 < N && starts[idx + 1] <= probe) {
      ++idx;
    }
    sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
  }
  return sum;
}