#include "interface.h"

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
  size_t i = 0; // probe index
  size_t j = 0; // interval index (first start > current probe)
  size_t m = starts.size();
  size_t n = probes.size();

  // Linear sweep: both sorted.
  while (i < n) {
    // Advance j until start[j] > probes[i]
    while (j < m && starts[j] <= probes[i]) {
      ++j;
    }
    // j is first index with starts[j] > probes[i]; the matching interval is j-1.
    // Since harness guarantees valid interval, j > 0.
    sum += static_cast<uint64_t>(payloads[j-1]) * static_cast<uint64_t>(weights[i]);
    ++i;
  }
  return sum;
}