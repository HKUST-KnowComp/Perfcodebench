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
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::deque<std::size_t> dq;
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < values.size(); ++i) {
      while (!dq.empty() && dq.front() + static_cast<std::size_t>(window) <= i) {
        dq.pop_front();
      }
      while (!dq.empty() && values[dq.back()] <= values[i]) {
        dq.pop_back();
      }
      dq.push_back(i);
      if (i + 1 >= static_cast<std::size_t>(window)) {
        hash = mix(hash, values[dq.front()]);
      }
    }
  }
  return hash;
}
