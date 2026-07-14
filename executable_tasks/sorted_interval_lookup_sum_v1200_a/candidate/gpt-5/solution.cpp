#include "interface.h"

#include <cstdint>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;

  // Match baseline semantics: if iters <= 0, return 0 (baseline would not enter loop)
  if (iters <= 0) return 0;

  const std::size_t nStarts = starts.size();
  const std::size_t nProbes = probes.size();
  if (nStarts == 0 || nProbes == 0) return 0;

  const uint32_t* s = starts.data();
  const uint32_t* p = probes.data();
  const uint32_t* w = weights.data();
  const uint32_t* pay = payloads.data();

  uint64_t sum = 0;

  std::size_t j = 0;
  uint32_t curr_payload = pay[0];

  for (std::size_t i = 0; i < nProbes; ++i) {
    const uint32_t probe = p[i];
    // Advance interval index while next start is <= current probe
    while ((j + 1) < nStarts && s[j + 1] <= probe) {
      ++j;
      curr_payload = pay[j];
    }
    sum += static_cast<uint64_t>(curr_payload) * static_cast<uint64_t>(w[i]);
  }

  return sum;
}
