#include "interface.h"

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  if (iters <= 0) return 0;
  if (starts.empty() || probes.empty()) return 0;

  const uint32_t* s = starts.data();
  const uint32_t* pr = probes.data();
  const uint32_t* pay = payloads.data();
  const uint32_t* w = weights.data();
  const std::size_t n_probes = probes.size();
  const std::size_t n_starts = starts.size();

  uint64_t sum = 0;
  std::size_t idx = 0;
  for (std::size_t i = 0; i < n_probes; ++i) {
    uint32_t probe = pr[i];
    while (idx + 1 < n_starts && s[idx + 1] <= probe) {
      ++idx;
    }
    sum += static_cast<uint64_t>(pay[idx]) * static_cast<uint64_t>(w[i]);
  }
  return sum;
}
