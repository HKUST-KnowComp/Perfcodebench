#include "interface.h"

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  const uint32_t* b = boundaries.data();
  const uint32_t* p = probes.data();
  const size_t nb = boundaries.size();
  const size_t np = probes.size();

  // Detect if probes are non-decreasing to use linear merge.
  bool probes_sorted = true;
  for (size_t i = 1; i < np; ++i) {
    if (p[i] < p[i - 1]) { probes_sorted = false; break; }
  }

  uint64_t per_iter_sum = 0;

  if (probes_sorted) {
    // Linear sweep using two pointers; also collapse runs of equal probes.
    size_t j = 0; // index into boundaries: count of <= current probe
    size_t i = 0; // index into probes
    while (i < np) {
      const uint32_t val = p[i];
      // Advance boundary pointer while boundaries[j] <= val
      while (j < nb && b[j] <= val) {
        ++j;
      }
      // Count run length of equal probes
      size_t k = i + 1;
      while (k < np && p[k] == val) {
        ++k;
      }
      const uint64_t run = static_cast<uint64_t>(k - i);
      per_iter_sum += run * static_cast<uint64_t>(j);
      i = k;
    }
  } else {
    // Fallback: per-probe binary search (upper_bound) implemented inline.
    for (size_t i = 0; i < np; ++i) {
      uint32_t val = p[i];
      size_t lo = 0, hi = nb;
      while (lo < hi) {
        size_t mid = (lo + hi) >> 1;
        if (b[mid] <= val) {
          lo = mid + 1;
        } else {
          hi = mid;
        }
      }
      per_iter_sum += static_cast<uint64_t>(lo);
    }
  }

  return per_iter_sum * static_cast<uint64_t>(iters);
}
