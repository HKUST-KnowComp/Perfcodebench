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
  std::vector<uint32_t> partitioned;
  partitioned.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i]) {
      partitioned.push_back(values[i]);
    }
  }
  for (std::size_t i = 0; i < n; ++i) {
    if (!flags[i]) {
      partitioned.push_back(values[i]);
    }
  }
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (uint32_t v : partitioned) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
