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

  const std::size_t n = probes.size();
  const std::size_t m = starts.size();
  if (iters <= 0 || n == 0 || m == 0) return 0;

  const uint32_t* __restrict s = starts.data();
  const uint32_t* __restrict p = payloads.data();
  const uint32_t* __restrict q = probes.data();
  const uint32_t* __restrict w = weights.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t idx = 0;
    uint32_t current_payload = p[0];

    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t probe = q[i];

      while (idx + 1 < m && s[idx + 1] <= probe) {
        ++idx;
        current_payload = p[idx];
      }

      local_sum += static_cast<uint64_t>(current_payload) * static_cast<uint64_t>(w[i]);
    }

    sum = local_sum;
  }

  return sum;
}
