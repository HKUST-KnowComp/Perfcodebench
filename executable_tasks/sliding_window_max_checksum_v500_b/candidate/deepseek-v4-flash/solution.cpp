#include "interface.h"
#include <deque>

namespace {
uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  uint64_t hash = 1469598103934665603ULL;
  std::size_t n = values.size();
  std::size_t w = static_cast<std::size_t>(window);
  std::deque<std::size_t> dq;

  for (int iter = 0; iter < iters; ++iter) {
    dq.clear();
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      while (!dq.empty() && dq.front() <= i - w)
        dq.pop_front();
      while (!dq.empty() && values[dq.back()] <= values[i])
        dq.pop_back();
      dq.push_back(i);
      if (i >= w - 1) {
        hash = mix(hash, values[dq.front()]);
      }
    }
  }
  return hash;
}