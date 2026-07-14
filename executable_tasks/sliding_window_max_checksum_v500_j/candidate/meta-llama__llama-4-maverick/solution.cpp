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
    std::deque<std::size_t> max_indices;
    for (std::size_t i = 0; i < values.size(); ++i) {
      while (!max_indices.empty() && max_indices.front() <= i - static_cast<std::size_t>(window)) {
        max_indices.pop_front();
      }
      while (!max_indices.empty() && values[max_indices.back()] <= values[i]) {
        max_indices.pop_back();
      }
      max_indices.push_back(i);
      if (i >= static_cast<std::size_t>(window) - 1) {
        hash = mix(hash, values[max_indices.front()]);
      }
    }
  }
  return hash;
}