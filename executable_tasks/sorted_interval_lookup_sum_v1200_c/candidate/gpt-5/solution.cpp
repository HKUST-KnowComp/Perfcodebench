#include "interface.h"

#include <algorithm>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int /*iters*/) {
  const std::size_t n = starts.size();
  if (n == 0 || probes.empty()) {
    return 0ull;
  }

  const uint32_t* s = starts.data();
  const uint32_t* e = ends.data();
  const uint32_t* pl = payloads.data();
  const uint32_t* w = weights.data();

  uint64_t sum = 0;
  for (std::size_t i = 0; i < probes.size(); ++i) {
    const uint32_t x = probes[i];
    const uint32_t* ub = std::upper_bound(s, s + n, x);
    if (ub == s) {
      continue; // x is before the first interval start
    }
    std::size_t idx = static_cast<std::size_t>(ub - s - 1);
    // Verify that the probe lies within the interval [starts[idx], ends[idx]]
    if (x <= e[idx]) {
      sum += static_cast<uint64_t>(pl[idx]) * static_cast<uint64_t>(w[i]);
    }
  }

  return sum;
}
