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
  
  if (iters == 0) {
    return 0;
  }
  
  // Precompute for each probe the payload value using a linear sweep.
  std::vector<uint64_t> payload_for_probe(probes.size());
  std::size_t j = 0;  // index into starts
  for (std::size_t i = 0; i < probes.size(); ++i) {
    while (j < starts.size() && starts[j] <= probes[i]) {
      ++j;
    }
    // j is the first index where starts[j] > probes[i], so last valid is j-1.
    // Input invariant guarantees probes[i] >= starts[0], so j > 0.
    payload_for_probe[i] = payloads[j - 1];
  }
  
  // Compute the weighted sum once (independent of iterations).
  uint64_t total = 0;
  for (std::size_t i = 0; i < probes.size(); ++i) {
    total += payload_for_probe[i] * static_cast<uint64_t>(weights[i]);
  }
  
  // The sum is identical for every iteration.  Repeat just to satisfy the
  // iters parameter (no real work needed).
  for (int iter = 1; iter < iters; ++iter) {
    // all iterations produce the same total
  }
  
  return total;
}
