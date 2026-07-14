#include "interface.h"

#include <cstddef>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) return 0;
  const size_t nb = boundaries.size();
  const size_t np = probes.size();
  if (np == 0 || nb == 0) {
    // If no probes or no boundaries, sum is zero.
    return 0;
  }

  const uint32_t* bp = boundaries.data();
  const uint32_t* pp = probes.data();

  // Check if probes are nondecreasing; if not, fall back to per-probe binary search.
  bool nondecreasing = true;
  for (size_t i = 1; i < np; ++i) {
    if (pp[i] < pp[i - 1]) { nondecreasing = false; break; }
  }

  uint64_t sum = 0;

  if (nondecreasing) {
    const uint32_t* bcur = bp;
    const uint32_t* bend = bp + nb;
    for (size_t i = 0; i < np; ++i) {
      const uint32_t v = pp[i];
      while (bcur != bend && *bcur <= v) {
        ++bcur;
      }
      sum += static_cast<uint64_t>(bcur - bp);
      if (bcur == bend) {
        // All remaining probes map to bucket id == nb
        sum += static_cast<uint64_t>(nb) * static_cast<uint64_t>(np - i - 1);
        break;
      }
    }
  } else {
    // Fallback: manual upper_bound per probe (handles arbitrary probe order)
    for (size_t i = 0; i < np; ++i) {
      const uint32_t v = pp[i];
      size_t lo = 0, hi = nb;
      while (lo < hi) {
        size_t mid = lo + ((hi - lo) >> 1);
        if (boundaries[mid] <= v) {
          lo = mid + 1;
        } else {
          hi = mid;
        }
      }
      sum += static_cast<uint64_t>(lo);
    }
  }

  return sum;
}
