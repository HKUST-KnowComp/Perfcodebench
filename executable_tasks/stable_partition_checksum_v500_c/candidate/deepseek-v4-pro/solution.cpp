#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) return 0;
  const uint64_t seed = 1469598103934665603ULL;
  uint64_t hash = seed;
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (flags[i]) {
      hash = mix(hash, values[i]);
    }
  }
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (!flags[i]) {
      hash = mix(hash, values[i]);
    }
  }
  return hash;
}
