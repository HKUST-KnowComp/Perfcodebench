#include "interface.h"
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const std::size_t n = values.size();
  std::vector<std::size_t> idx;
  idx.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i]) idx.push_back(i);
  }
  for (std::size_t i = 0; i < n; ++i) {
    if (!flags[i]) idx.push_back(i);
  }

  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t pos : idx) {
      hash = mix(hash, values[pos]);
    }
  }
  return hash;
}