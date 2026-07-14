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
  const std::size_t n = starts.size();
  const uint32_t* __restrict s = starts.data();
  const uint32_t* __restrict p = payloads.data();
  const std::size_t m = probes.size();
  const uint32_t* __restrict pr = probes.data();
  const uint32_t* __restrict w = weights.data();

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    // process two probes per iteration
    for (; i + 1 < m; i += 2) {
      const uint32_t v0 = pr[i];
      const uint32_t v1 = pr[i + 1];

      // ---------- binary search for v0 ----------
      std::size_t lo0 = 0;
      std::size_t hi0 = n;
      while (hi0 - lo0 > 4) {
        std::size_t mid = (lo0 + hi0) >> 1;
        if (s[mid] <= v0) {
          lo0 = mid;
        } else {
          hi0 = mid;
        }
      }
      while (lo0 < hi0 && s[lo0] <= v0) ++lo0;
      const std::size_t idx0 = lo0 - 1;

      // ---------- binary search for v1 ----------
      std::size_t lo1 = 0;
      std::size_t hi1 = n;
      while (hi1 - lo1 > 4) {
        std::size_t mid = (lo1 + hi1) >> 1;
        if (s[mid] <= v1) {
          lo1 = mid;
        } else {
          hi1 = mid;
        }
      }
      while (lo1 < hi1 && s[lo1] <= v1) ++lo1;
      const std::size_t idx1 = lo1 - 1;

      sum += static_cast<uint64_t>(p[idx0]) * w[i];
      sum += static_cast<uint64_t>(p[idx1]) * w[i + 1];
    }
    // tail: handle odd count
    for (; i < m; ++i) {
      const uint32_t v = pr[i];
      std::size_t lo = 0;
      std::size_t hi = n;
      while (hi - lo > 4) {
        std::size_t mid = (lo + hi) >> 1;
        if (s[mid] <= v) {
          lo = mid;
        } else {
          hi = mid;
        }
      }
      while (lo < hi && s[lo] <= v) ++lo;
      const std::size_t idx = lo - 1;
      sum += static_cast<uint64_t>(p[idx]) * w[i];
    }
  }
  return sum;
}