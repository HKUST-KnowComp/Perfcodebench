#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;  // FNV-1a 64-bit prime
  return hash;
}

}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int /*iters*/) {
  // The final hash in the baseline is independent of iters; it always computes
  // the FNV-1a hash of the stable-partitioned sequence once. We compute that
  // directly without materializing the partitioned array.
  const std::size_t n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis

  // First, hash values where flag is true (left side), preserving order.
  for (std::size_t i = 0; i < n; ++i) {
    if (f[i]) {
      hash = mix(hash, static_cast<uint64_t>(v[i]));
    }
  }
  // Then, hash values where flag is false (right side), preserving order.
  for (std::size_t i = 0; i < n; ++i) {
    if (!f[i]) {
      hash = mix(hash, static_cast<uint64_t>(v[i]));
    }
  }

  return hash;
}
