#include "interface.h"
#include <cstdint>
#include <vector>
#include <deque>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  const int n = static_cast<int>(values.size());
  const int w = window;
  if (w <= 0 || w > n) return 0;

  // Reusable deque across iterations to avoid allocations
  std::deque<int> dq;
  dq.reserve(w);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    dq.clear();
    hash = 1469598103934665603ULL;

    for (int i = 0; i < n; ++i) {
      // Pop indices whose values are <= current value
      while (!dq.empty() && values[dq.back()] <= values[i])
        dq.pop_back();

      dq.push_back(i);

      // Remove indices that are out of the current window
      while (!dq.empty() && dq.front() <= i - w)
        dq.pop_front();

      // The window is fully populated once i >= w - 1
      if (i >= w - 1) {
        uint32_t mx = values[dq.front()];
        hash = mix(hash, mx);
      }
    }
  }
  return hash;
}