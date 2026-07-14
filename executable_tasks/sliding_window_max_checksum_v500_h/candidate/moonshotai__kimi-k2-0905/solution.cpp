#include "interface.h"
#include <deque>
#include <vector>
#include <cstdint>

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

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::deque<std::size_t> dq;

    for (std::size_t i = 0; i < n; ++i) {
      // Pop from back while current value is greater
      while (!dq.empty() && values[i] >= values[dq.back()])
        dq.pop_back();
      dq.push_back(i);

      // Pop from front if outside window
      while (dq.front() <= i - w)
        dq.pop_front();

      // Once window is fully formed, use front as max
      if (i >= w - 1)
        hash = mix(hash, values[dq.front()]);
    }
  }
  return hash;
}