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
  const size_t n = probes.size();
  const size_t m = starts.size();

  // Build permutation: sort probes while remembering original positions
  struct Entry {
    uint32_t probe;
    uint32_t weight;
    uint32_t idx; // original position to restore order
  };
  std::vector<Entry> tmp(n);
  for (size_t i = 0; i < n; ++i) {
    tmp[i] = {probes[i], weights[i], static_cast<uint32_t>(i)};
  }
  std::sort(tmp.begin(), tmp.end(),
            [](const Entry& a, const Entry& b) { return a.probe < b.probe; });

  std::vector<uint64_t> partial(n);

  for (int iter = 0; iter < iters; ++iter) {
    size_t j = 0; // interval pointer
    for (size_t i = 0; i < n; ++i) {
      const uint32_t p = tmp[i].probe;
      // advance j until the probe lies inside the interval [starts[j], ends[j])
      while (j < m && ends[j] <= p) ++j;
      uint64_t payload = 0;
      if (j < m && starts[j] <= p) {
        payload = payloads[j];
      }
      partial[tmp[i].idx] = payload * tmp[i].weight;
    }
  }

  uint64_t sum = 0;
  for (uint64_t v : partial) sum += v;
  return sum;
}