#include "interface.h"

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
    int /*iters*/) {
  uint64_t hash = 1469598103934665603ULL;
  const std::size_t n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();
  for (std::size_t i = 0; i < n; ++i) {
    if (f[i]) {
      hash = mix(hash, v[i]);
    }
  }
  for (std::size_t i = 0; i < n; ++i) {
    if (!f[i]) {
      hash = mix(hash, v[i]);
    }
  }
  return hash;
}
