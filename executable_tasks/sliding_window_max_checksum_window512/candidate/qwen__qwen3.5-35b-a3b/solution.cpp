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
    hash = 1469598103934665603ULL;
    std::deque<size_t> dq;
    for (size_t j = 0; j < values.size(); ++j) {
      while (!dq.empty() && values[dq.back()] <= values[j]) {
        dq.pop_back();
      }
      dq.push_back(j);
      if (dq.front() == j - static_cast<size_t>(window)) {
        dq.pop_front();
      }
      if (j >= static_cast<size_t>(window) - 1) {
        uint32_t mx = values[dq.front()];
        hash = mix(hash, mx);
      }
    }
  }
  return hash;
}