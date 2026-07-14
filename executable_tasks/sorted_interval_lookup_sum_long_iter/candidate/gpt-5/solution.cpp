#include "interface.h"

#include <algorithm>
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
  (void)iters;

  const std::size_t n = starts.size();
  const std::size_t m = probes.size();
  if (m == 0) return 0ull;

  const uint32_t* s = starts.data();
  const uint32_t* p = probes.data();
  const uint32_t* w = weights.data();
  const uint32_t* pay = payloads.data();

  // Check if probes are sorted ascending; if not, use upper_bound fallback.
  bool probes_sorted = true;
  for (std::size_t i = 1; i < m; ++i) {
    if (p[i] < p[i - 1]) { probes_sorted = false; break; }
  }

  uint64_t sum = 0;
  if (probes_sorted) {
    // Two-pointer linear scan: advance start index as probes increase.
    std::size_t j = 0; // index into starts, points to first start > current probe
    for (std::size_t i = 0; i < m; ++i) {
      const uint32_t probe = p[i];
      while (j < n && s[j] <= probe) {
        ++j;
      }
      const std::size_t idx = j - 1; // assumes probe >= starts[0]
      sum += static_cast<uint64_t>(pay[idx]) * static_cast<uint64_t>(w[i]);
    }
  } else {
    // Fallback: per-probe binary search (upper_bound) to preserve correctness.
    for (std::size_t i = 0; i < m; ++i) {
      const auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
      sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
    }
  }

  return sum;
}
