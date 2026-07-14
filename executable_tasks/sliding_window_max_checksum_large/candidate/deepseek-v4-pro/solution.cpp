#include "interface.h"
#include <deque>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  const std::size_t n = values.size();
  std::vector<uint32_t> maxes;

  if (window > 0 && static_cast<std::size_t>(window) <= n) {
    maxes.reserve(n - static_cast<std::size_t>(window) + 1);
    std::deque<std::size_t> dq;
    for (std::size_t i = 0; i < n; ++i) {
      while (!dq.empty() && dq.front() + static_cast<std::size_t>(window) <= i)
        dq.pop_front();
      while (!dq.empty() && values[dq.back()] <= values[i])
        dq.pop_back();
      dq.push_back(i);
      if (i + 1 >= static_cast<std::size_t>(window))
        maxes.push_back(values[dq.front()]);
    }
  } else if (window == 0) {
    maxes.assign(n + 1, 0);
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t mx : maxes) {
      hash = mix(hash, mx);
    }
  }
  return hash;
}
