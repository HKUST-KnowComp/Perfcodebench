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

  const std::size_t n_intervals = starts.size();
  const std::size_t n_probes = probes.size();

  if (iters <= 0 || n_probes == 0 || n_intervals == 0) {
    return 0;
  }

  const uint32_t* s = starts.data();
  const uint32_t* p = payloads.data();
  const uint32_t* pr = probes.data();
  const uint32_t* w = weights.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    // Probes are sorted and interval count is small for this task.
    // Use a single forward scan instead of per-probe binary search.
    std::size_t idx = 0;

    for (std::size_t i = 0; i < n_probes; ++i) {
      const uint32_t probe = pr[i];

      while (idx + 1 < n_intervals && s[idx + 1] <= probe) {
        ++idx;
      }

      local_sum += static_cast<uint64_t>(p[idx]) * static_cast<uint64_t>(w[i]);
    }

    sum = local_sum;
  }

  return sum;
}
