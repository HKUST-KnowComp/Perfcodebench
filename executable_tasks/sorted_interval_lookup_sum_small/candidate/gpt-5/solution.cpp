#include "interface.h"

#include <cstddef>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  (void)iters;

  const std::size_t n_intervals = starts.size();
  const std::size_t n_probes = probes.size();

  if (n_intervals == 0 || n_probes == 0) {
    return 0;
  }

  // Detect if probes are non-decreasing; if so, we can use a linear scan.
  bool probes_sorted = true;
  for (std::size_t i = 1; i < n_probes; ++i) {
    if (probes[i] < probes[i - 1]) {
      probes_sorted = false;
      break;
    }
  }

  uint64_t sum = 0;

  const uint32_t* sp = starts.data();
  const uint32_t* pp = probes.data();
  const uint32_t* wp = weights.data();
  const uint32_t* pay = payloads.data();

  if (probes_sorted) {
    // Linear two-pointer scan over starts and probes.
    std::size_t idx = 0;
    for (std::size_t i = 0; i < n_probes; ++i) {
      const uint32_t p = pp[i];
      while ((idx + 1) < n_intervals && sp[idx + 1] <= p) {
        ++idx;
      }
      sum += static_cast<uint64_t>(pay[idx]) * static_cast<uint64_t>(wp[i]);
    }
  } else {
    // Fallback: per-probe binary search for last start <= probe.
    for (std::size_t i = 0; i < n_probes; ++i) {
      const uint32_t p = pp[i];
      std::size_t lo = 0, hi = n_intervals;
      // Find first index > p, then subtract 1.
      while (lo < hi) {
        std::size_t mid = (lo + hi) >> 1;
        if (sp[mid] <= p) {
          lo = mid + 1;
        } else {
          hi = mid;
        }
      }
      std::size_t idx = (lo == 0) ? 0 : (lo - 1);
      sum += static_cast<uint64_t>(pay[idx]) * static_cast<uint64_t>(wp[i]);
    }
  }

  return sum;
}
