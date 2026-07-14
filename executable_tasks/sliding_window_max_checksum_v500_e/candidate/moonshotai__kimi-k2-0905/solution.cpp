#include "interface.h"
#include <deque>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  const std::size_t n = values.size();
  const std::size_t w = static_cast<std::size_t>(window);
  if (w == 0 || w > n) return 0;

  std::deque<std::size_t> dq;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    dq.clear();
    hash = 1469598103934665603ULL;

    for (std::size_t i = 0; i < n; ++i) {
      // Remove indices outside the left boundary of the current window
      while (!dq.empty() && dq.front() <= i - w) {
        dq.pop_front();
      }
      // Remove indices whose values are <= current value from the back
      while (!dq.empty() && values[dq.back()] <= values[i]) {
        dq.pop_back();
      }
      dq.push_back(i);

      // Once the window is fully formed, the max is at the front of the deque
      if (i >= w - 1) {
        hash = mix(hash, values[dq.front()]);
      }
    }
  }
  return hash;
}