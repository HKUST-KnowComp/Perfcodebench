#include "interface.h"

namespace {
uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  std::size_t n = values.size();
  if (window <= 0 || n == 0) return 0;
  std::size_t m = n - window + 1;
  std::vector<uint32_t> maxes(m);
  // monotonic deque implemented with vector and head index
  std::vector<std::size_t> dq;
  std::size_t head = 0;
  for (std::size_t i = 0; i < n; ++i) {
    // maintain decreasing order of values
    while (dq.size() > head && values[dq.back()] <= values[i]) {
      dq.pop_back();
    }
    dq.push_back(i);
    // remove indices out of window
    if (dq.size() > head && dq[head] + std::size_t(window) <= i) {
      ++head;
    }
    // record maximum when window is full
    if (i >= std::size_t(window) - 1) {
      maxes[i - window + 1] = values[dq[head]];
    }
  }
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t k = 0; k < m; ++k) {
      hash = mix(hash, maxes[k]);
    }
  }
  return hash;
}
