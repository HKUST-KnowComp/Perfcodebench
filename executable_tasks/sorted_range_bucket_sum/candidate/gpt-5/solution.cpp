#include "interface.h"

#include <algorithm>
#include <cstdint>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  // Fast path when both inputs are sorted: single linear merge pass.
  uint64_t base_sum = 0;
  const size_t nB = boundaries.size();
  const size_t nP = probes.size();

  if (std::is_sorted(boundaries.begin(), boundaries.end()) &&
      std::is_sorted(probes.begin(), probes.end())) {
    const uint32_t* b = boundaries.data();
    const uint32_t* be = b + nB;
    const uint32_t* bp = b; // moving pointer within boundaries

    const uint32_t* p = probes.data();
    const uint32_t* pe = p + nP;

    while (p < pe) {
      uint32_t v = *p++;
      while (bp < be && *bp <= v) {
        ++bp;
      }
      base_sum += static_cast<uint64_t>(bp - b);
    }
  } else {
    // Fallback: handle any input order using binary search per probe.
    const uint32_t* b = boundaries.data();
    const uint32_t* be = b + nB;
    for (size_t i = 0; i < nP; ++i) {
      const uint32_t v = probes[i];
      const uint32_t* up = std::upper_bound(b, be, v);
      base_sum += static_cast<uint64_t>(up - b);
    }
  }

  return base_sum * static_cast<uint64_t>(iters);
}
