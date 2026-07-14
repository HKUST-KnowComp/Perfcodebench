#include "interface.h"

#include <vector>

namespace {

uint64_t checksum_u64(const std::vector<uint64_t>& values) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint64_t v : values) {
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t window_sum_checksum(const std::vector<uint32_t>& values, int window, int iters) {
  std::vector<uint64_t> out(values.size(), 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t running = 0;
    for (std::size_t i = 0; i < values.size(); ++i) {
      running += static_cast<uint64_t>(values[i]);
      if (i >= static_cast<std::size_t>(window)) {
        running -= static_cast<uint64_t>(values[i - static_cast<std::size_t>(window)]);
      }
      out[i] = running;
    }
    hash = checksum_u64(out);
  }
  return hash;
}
