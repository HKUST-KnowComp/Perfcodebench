#include "interface.h"

#include <cstdint>
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
  if (iters <= 0) {
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = values.size();

  // First pass: elements where flag is true, in original order
  for (std::size_t i = 0; i < n; ++i) {
    if (flags[i]) {
      hash = mix(hash, values[i]);
    }
  }

  // Second pass: elements where flag is false, preserving order
  for (std::size_t i = 0; i < n; ++i) {
    if (!flags[i]) {
      hash = mix(hash, values[i]);
    }
  }

  return hash;
}