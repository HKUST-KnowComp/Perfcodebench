#include "interface.h"
#include <vector>
#include <deque>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  if (values.empty() || window <= 0 || static_cast<std::size_t>(window) > values.size()) {
    return 0;
  }

  std::size_t n = values.size();
  std::size_t w = static_cast<std::size_t>(window);
  std::vector<uint32_t> max_elements;
  max_elements.reserve(n - w + 1);

  // Monotonic queue to find sliding window maximums in O(N)
  std::deque<std::size_t> dq;
  for (std::size_t i = 0; i < n; ++i) {
    // Remove indices out of window
    if (!dq.empty() && dq.front() <= i - w) {
      dq.pop_front();
    }
    // Remove elements smaller than current element
    while (!dq.empty() && values[dq.back()] <= values[i]) {
      dq.pop_back();
    }
    dq.push_back(i);

    // If window is full, the front is the max
    if (i >= w - 1) {
      max_elements.push_back(values[dq.front()]);
    }
  }

  uint64_t hash = 0;
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET_BASIS;
    for (uint32_t mx : max_elements) {
      hash = mix(hash, static_cast<uint64_t>(mx));
    }
  }

  return hash;
}
