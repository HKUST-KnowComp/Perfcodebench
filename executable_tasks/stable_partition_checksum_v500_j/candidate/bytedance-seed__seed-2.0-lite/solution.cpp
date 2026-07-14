#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

static constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

} // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  const size_t n = values.size();
  std::vector<uint32_t> partitioned(n);
  size_t write_idx = 0;

  // Build stable partition once, preserve original order of true/false groups
  for (size_t i = 0; i < n; ++i) {
    if (flags[i]) {
      partitioned[write_idx++] = values[i];
    }
  }
  for (size_t i = 0; i < n; ++i) {
    if (!flags[i]) {
      partitioned[write_idx++] = values[i];
    }
  }

  // Run hash computation only, no repeated partition work
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    for (uint32_t v : partitioned) {
      hash = mix(hash, v);
    }
  }
  return hash;
}