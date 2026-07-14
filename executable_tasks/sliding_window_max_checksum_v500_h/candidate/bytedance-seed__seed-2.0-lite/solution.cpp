#include "interface.h"
#include <deque>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

};

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  uint64_t hash = 0;
  const std::size_t window_sz = static_cast<std::size_t>(window);
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::deque<std::size_t> dq;
    for (std::size_t i = 0; i < values.size(); ++i) {
      while (!dq.empty() && values[i] >= values[dq.back()]) {
        dq.pop_back();
      }
      dq.push_back(i);
      while (dq.front() <= i - window_sz) {
        dq.pop_front();
      }
      if (i >= window_sz - 1) {
        hash = mix(hash, values[dq.front()]);
      }
    }
  }
  return hash;
}