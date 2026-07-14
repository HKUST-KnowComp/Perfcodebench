#include "interface.h"

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  // Match baseline: if no iterations, return 0
  if (iters <= 0) {
    return 0ULL;
  }

  const std::size_t n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis

  // First pass: elements with flag true (stable order)
  for (std::size_t i = 0; i < n; ++i) {
    if (f[i]) {
      hash ^= static_cast<uint64_t>(v[i]);
      hash *= 1099511628211ULL;  // FNV prime
    }
  }

  // Second pass: elements with flag false (stable order)
  for (std::size_t i = 0; i < n; ++i) {
    if (!f[i]) {
      hash ^= static_cast<uint64_t>(v[i]);
      hash *= 1099511628211ULL;  // FNV prime
    }
  }

  return hash;
}
