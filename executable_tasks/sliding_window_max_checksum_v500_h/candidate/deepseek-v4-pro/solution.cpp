#include "interface.h"
#include <deque>
#include <cstdint>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  const int n = static_cast<int>(values.size());
  const uint64_t seed = 1469598103934665603ULL;

  // Handle edge cases where window <= 0
  if (window <= 0) {
    if (window == 0) {
      const int windows_count = n + 1;
      uint64_t hash = 0;
      for (int iter = 0; iter < iters; ++iter) {
        hash = seed;
        for (int i = 0; i < windows_count; ++i) {
          hash = mix(hash, 0);
        }
      }
      return hash;
    } else {
      // window < 0: no windows
      uint64_t hash = 0;
      for (int iter = 0; iter < iters; ++iter) {
        hash = seed;
      }
      return hash;
    }
  }

  // No windows if window > n
  if (window > n) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
      hash = seed;
    }
    return hash;
  }

  // Pre‑compute sliding window maxima once (O(n) with deque)
  std::vector<uint32_t> max_arr;
  max_arr.reserve(n - window + 1);
  std::deque<int> dq;

  for (int i = 0; i < n; ++i) {
    while (!dq.empty() && values[dq.back()] <= values[i])
      dq.pop_back();
    dq.push_back(i);

    if (dq.front() == i - window)
      dq.pop_front();

    if (i >= window - 1)
      max_arr.push_back(values[dq.front()]);
  }

  // Iterate, mixing the pre‑computed maxima each time
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = seed;
    for (uint32_t mx : max_arr) {
      hash = mix(hash, mx);
    }
  }

  return hash;
}