#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  const std::size_t n = values.size();
  std::vector<uint32_t> maxes;
  if (window > 0 && static_cast<std::size_t>(window) <= n) {
    maxes.reserve(n - static_cast<std::size_t>(window) + 1);
    std::vector<std::size_t> idx(n);
    std::size_t head = 0, tail = 0;
    for (std::size_t i = 0; i < n; ++i) {
      while (tail > head && values[idx[tail - 1]] <= values[i]) {
        --tail;
      }
      idx[tail++] = i;
      if (idx[head] + static_cast<std::size_t>(window) <= i) {
        ++head;
      }
      if (i + 1 >= static_cast<std::size_t>(window)) {
        maxes.push_back(values[idx[head]]);
      }
    }
  }

  const uint64_t base = 1469598103934665603ULL;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = base;
    for (std::size_t i = 0; i < maxes.size(); ++i) {
      hash = mix(hash, maxes[i]);
    }
  }
  return hash;
}
