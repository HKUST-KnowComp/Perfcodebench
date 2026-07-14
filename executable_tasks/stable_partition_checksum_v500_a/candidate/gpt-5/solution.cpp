#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t n = values.size();
  const uint8_t* f = flags.data();
  const uint32_t* v = values.data();

  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
  const uint64_t prime = 1099511628211ULL; // FNV-1a prime

  // First pass: flags true (non-zero), stable order
  for (std::size_t i = 0; i < n; ++i) {
    if (f[i]) {
      hash ^= static_cast<uint64_t>(v[i]);
      hash *= prime;
    }
  }
  // Second pass: flags false (zero), stable order
  for (std::size_t i = 0; i < n; ++i) {
    if (!f[i]) {
      hash ^= static_cast<uint64_t>(v[i]);
      hash *= prime;
    }
  }

  return hash;
}
