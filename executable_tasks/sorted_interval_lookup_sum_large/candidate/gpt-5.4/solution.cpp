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
  if (n == 0 || iters <= 0) return 0;

  const uint32_t* starts_data = starts.data();
  const uint32_t* payloads_data = payloads.data();
  const uint32_t* probes_data = probes.data();
  const uint32_t* weights_data = weights.data();
  const std::size_t starts_size = starts.size();

  uint64_t sum = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;

    std::size_t idx = 0;
    std::size_t i = 0;

    while (i < n) {
      const uint32_t p = probes_data[i];

      while (idx + 1 < starts_size && starts_data[idx + 1] <= p) {
        ++idx;
      }

      uint64_t payload = static_cast<uint64_t>(payloads_data[idx]);
      local_sum += payload * static_cast<uint64_t>(weights_data[i]);
      ++i;

      while (i < n && probes_data[i] == p) {
        local_sum += payload * static_cast<uint64_t>(weights_data[i]);
        ++i;
      }
    }

    sum = local_sum;
  }

  return sum;
}
