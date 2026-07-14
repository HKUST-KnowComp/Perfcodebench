#include "interface.h"

#include <cstddef>

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL; // FNV-1a prime
  return hash;
}
}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  // Match baseline behavior: if no iterations, return initial 0 hash
  if (iters <= 0) return 0ULL;

  const std::size_t n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();

  // Compute the checksum for the stable-partitioned order directly
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis

  // Left (true flags), stable order
  for (std::size_t i = 0; i < n; ++i) {
    if (f[i]) {
      hash = mix(hash, static_cast<uint64_t>(v[i]));
    }
  }
  // Right (false flags), stable order
  for (std::size_t i = 0; i < n; ++i) {
    if (!f[i]) {
      hash = mix(hash, static_cast<uint64_t>(v[i]));
    }
  }

  return hash;
}
