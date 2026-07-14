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
    for (std::size_t i = 0; i < values.size(); ++i) {
      uint64_t sum = 0;
      const std::size_t begin = (i + 1 < static_cast<std::size_t>(window)) ? 0 : (i + 1 - static_cast<std::size_t>(window));
      for (std::size_t j = begin; j <= i; ++j) {
        sum += static_cast<uint64_t>(values[j]);
      }
      out[i] = sum;
    }
    hash = checksum_u64(out);
  }
  return hash;
}
