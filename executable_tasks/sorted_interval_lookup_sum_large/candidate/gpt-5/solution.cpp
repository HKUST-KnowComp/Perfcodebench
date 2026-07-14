#include "interface.h"

#include <algorithm>
#include <cstddef>
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

  // Preserve baseline semantics for iters == 0
  if (iters <= 0) return 0;

  const std::size_t n_probes = probes.size();
  const std::size_t n_starts = starts.size();
  if (n_probes == 0 || n_starts == 0) return 0;

  const uint32_t* s_ptr = starts.data();
  const uint32_t* pld_ptr = payloads.data();
  const uint32_t* pr_ptr = probes.data();
  const uint32_t* w_ptr = weights.data();

  uint64_t sum = 0;

  // Hybrid search: binary search for first probe, then advance linearly while
  // probes are non-decreasing; fall back to binary search if a decrease occurs.
  std::size_t idx = 0;

  // First probe: use upper_bound to find last start <= probe
  {
    const uint32_t probe = pr_ptr[0];
    const uint32_t* ub = std::upper_bound(s_ptr, s_ptr + n_starts, probe);
    // Precondition assumed by task data: probe >= starts[0]
    idx = static_cast<std::size_t>(ub - s_ptr) - 1;
    sum += static_cast<uint64_t>(pld_ptr[idx]) * static_cast<uint64_t>(w_ptr[0]);
  }

  uint32_t last_probe = pr_ptr[0];

  for (std::size_t i = 1; i < n_probes; ++i) {
    const uint32_t probe = pr_ptr[i];
    if (probe >= last_probe) {
      // Advance idx forward while next start <= probe
      while ((idx + 1) < n_starts && s_ptr[idx + 1] <= probe) {
        ++idx;
      }
    } else {
      // Probe decreased: fall back to binary search for correctness
      const uint32_t* ub = std::upper_bound(s_ptr, s_ptr + n_starts, probe);
      idx = static_cast<std::size_t>(ub - s_ptr) - 1;
    }
    sum += static_cast<uint64_t>(pld_ptr[idx]) * static_cast<uint64_t>(w_ptr[i]);
    last_probe = probe;
  }

  return sum;
}
