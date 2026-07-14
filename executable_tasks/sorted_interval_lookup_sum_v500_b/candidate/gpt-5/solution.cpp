#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;

  const uint32_t* s = starts.data();
  const uint32_t* pl = payloads.data();
  const uint32_t* pr = probes.data();
  const uint32_t* w = weights.data();
  const std::size_t n = starts.size();
  const std::size_t m = probes.size();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t j = 0; // number of starts <= current probe
    for (std::size_t i = 0; i < m; ++i) {
      const uint32_t p = pr[i];
      while (j < n && s[j] <= p) {
        ++j;
      }
      const std::size_t idx = j - 1; // as in upper_bound(starts, p) - 1
      sum += static_cast<uint64_t>(pl[idx]) * static_cast<uint64_t>(w[i]);
    }
  }
  return sum;
}
