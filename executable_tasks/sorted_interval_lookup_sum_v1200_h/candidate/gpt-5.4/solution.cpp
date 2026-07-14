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
  if (n == 0 || m == 0 || iters <= 0) return 0;

  const uint32_t* starts_data = starts.data();
  const uint32_t* payloads_data = payloads.data();
  const uint32_t* probes_data = probes.data();
  const uint32_t* weights_data = weights.data();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;

    std::size_t idx = 0;
    uint32_t next_start = (m > 1) ? starts_data[1] : 0;

    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t p = probes_data[i];

      while (idx + 1 < m && p >= next_start) {
        ++idx;
        if (idx + 1 < m) next_start = starts_data[idx + 1];
      }

      sum += static_cast<uint64_t>(payloads_data[idx]) *
             static_cast<uint64_t>(weights_data[i]);
    }
  }

  return sum;
}
