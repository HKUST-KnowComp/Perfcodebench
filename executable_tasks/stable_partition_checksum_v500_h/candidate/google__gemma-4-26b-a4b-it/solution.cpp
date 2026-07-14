#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  if (values.empty()) {
    return 0;
  }

  const size_t n = values.size();
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Step 1: Count 'true' flags to find the partition boundary
    size_t true_count = 0;
    for (size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        true_count++;
      }
    }

    hash = FNV_OFFSET_BASIS;

    // Step 2: Process 'true' elements (they come first in stable partition)
    for (size_t i = 0; i < n; ++i) {
      if (flags[i]) {
        hash = mix(hash, values[i]);
      }
    }

    // Step 3: Process 'false' elements (they come second)
    for (size_t i = 0; i < n; ++i) {
      if (!flags[i]) {
        hash = mix(hash, values[i]);
      }
    }
  }

  return hash;
}
