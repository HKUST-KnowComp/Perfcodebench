#include "interface.h"

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  // Preserve baseline behavior: if iters == 0, return 0 (hash never initialized in baseline).
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  // First pass: elements with flag != 0 (stable order)
  for (std::size_t i = 0; i < n; ++i) {
    if (f[i]) {
      hash = mix(hash, static_cast<uint64_t>(v[i]));
    }
  }
  
  // Second pass: elements with flag == 0 (stable order)
  for (std::size_t i = 0; i < n; ++i) {
    if (!f[i]) {
      hash = mix(hash, static_cast<uint64_t>(v[i]));
    }
  }

  return hash;
}
