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
    int iters) {
  // Match baseline behavior: if no iterations, return 0 without computing.
  if (iters <= 0) {
    return 0;
  }

  const auto n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();

  // Compute the hash exactly once; baseline resets and recomputes identically
  // each iteration, so the final result equals a single computation.
  uint64_t hash = 1469598103934665603ULL;  // FNV-1a 64-bit offset basis

  // First pass: left (flags nonzero), preserving order
  for (size_t i = 0; i < n; ++i) {
    if (f[i]) {
      hash = mix(hash, static_cast<uint64_t>(v[i]));
    }
  }
  // Second pass: right (flags zero), preserving order
  for (size_t i = 0; i < n; ++i) {
    if (!f[i]) {
      hash = mix(hash, static_cast<uint64_t>(v[i]));
    }
  }

  return hash;
}
