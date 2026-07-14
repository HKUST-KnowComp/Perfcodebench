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

  const std::size_t n = starts.size();
  const std::size_t m = probes.size();
  if (iters <= 0 || m == 0 || n == 0) return 0;

  const uint32_t* s = starts.data();
  const uint32_t* p = payloads.data();
  const uint32_t* q = probes.data();
  const uint32_t* w = weights.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t idx = 0;
    for (std::size_t i = 0; i < m; ++i) {
      const uint32_t probe = q[i];
      while (idx + 1 < n && s[idx + 1] <= probe) {
        ++idx;
      }
      local_sum += static_cast<uint64_t>(p[idx]) * static_cast<uint64_t>(w[i]);
    }

    sum = local_sum;
  }

  return sum;
}
