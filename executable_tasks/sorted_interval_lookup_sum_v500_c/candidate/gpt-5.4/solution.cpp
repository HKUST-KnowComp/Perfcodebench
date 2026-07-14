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
  if (iters <= 0 || n == 0) return 0;

  const uint32_t* const starts_data = starts.data();
  const uint32_t* const payloads_data = payloads.data();
  const uint32_t* const probes_data = probes.data();
  const uint32_t* const weights_data = weights.data();
  const std::size_t m = starts.size();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;

    std::size_t idx = 0;
    uint32_t current_payload = payloads_data[0];

    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t probe = probes_data[i];

      while (idx + 1 < m && starts_data[idx + 1] <= probe) {
        ++idx;
        current_payload = payloads_data[idx];
      }

      sum += static_cast<uint64_t>(current_payload) *
             static_cast<uint64_t>(weights_data[i]);
    }
  }

  return sum;
}
