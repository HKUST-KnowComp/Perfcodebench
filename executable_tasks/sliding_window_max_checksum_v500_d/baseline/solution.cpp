#include "interface.h"

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
    for (std::size_t i = 0; i + static_cast<std::size_t>(window) <= values.size(); ++i) {
      uint32_t mx = 0;
      for (int j = 0; j < window; ++j) {
        if (values[i + static_cast<std::size_t>(j)] > mx) {
          mx = values[i + static_cast<std::size_t>(j)];
        }
      }
      hash = mix(hash, mx);
    }
  }
  return hash;
}
