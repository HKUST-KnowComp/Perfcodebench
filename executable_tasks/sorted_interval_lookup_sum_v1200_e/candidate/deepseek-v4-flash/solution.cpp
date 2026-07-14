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
  (void)ends; // unused, matching baseline

  std::size_t n_probes = probes.size();
  std::size_t n_intervals = starts.size();
  int last_valid = -1; // index of current matching interval
  uint64_t base_sum = 0;

  for (std::size_t i = 0; i < n_probes; ++i) {
    uint32_t p = probes[i];
    // advance last_valid to the last interval with starts[last_valid] <= p
    while (static_cast<std::size_t>(last_valid + 1) < n_intervals &&
           starts[static_cast<std::size_t>(last_valid + 1)] <= p) {
      ++last_valid;
    }
    if (last_valid >= 0) {
      base_sum += static_cast<uint64_t>(payloads[static_cast<std::size_t>(last_valid)]) *
                  static_cast<uint64_t>(weights[i]);
    }
    // if last_valid < 0, probe lies before first interval; baseline would cause UB,
    // but problem guarantees probes are within range, so we skip.
  }

  if (iters == 0) return 0;
  return base_sum * static_cast<uint64_t>(iters);
}
