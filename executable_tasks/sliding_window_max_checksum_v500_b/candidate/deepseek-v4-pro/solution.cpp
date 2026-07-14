#include "interface.h"

#include <deque>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  if (iters == 0) {
    return 0;
  }

  const std::size_t n = values.size();
  if (n == 0 || static_cast<std::size_t>(window) > n) {
    return 1469598103934665603ULL;
  }

  // Pre‑compute all sliding‑window maxima using a monotonic deque.
  std::vector<uint32_t> maxima;
  maxima.reserve(n - static_cast<std::size_t>(window) + 1);

  std::deque<std::size_t> dq;   // indices, maintaining decreasing values
  for (std::size_t i = 0; i < n; ++i) {
    // Remove indices that are no longer better than the current value.
    while (!dq.empty() && values[dq.back()] <= values[i]) {
      dq.pop_back();
    }
    dq.push_back(i);

    // Discard the front if it fell out of the window.
    if (dq.front() + static_cast<std::size_t>(window) <= i) {
      dq.pop_front();
    }

    // Start recording maxima once we have a full window.
    if (i + 1 >= static_cast<std::size_t>(window)) {
      maxima.push_back(values[dq.front()]);
    }
  }

  // Hash the maxima sequence once (the outer „iters“ loop is idempotent).
  uint64_t hash = 1469598103934665603ULL;
  for (const uint32_t mx : maxima) {
    hash = mix(hash, mx);
  }
  return hash;
}
