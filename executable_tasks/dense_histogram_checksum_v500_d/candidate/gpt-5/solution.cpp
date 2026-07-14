#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Preserve baseline behavior for zero iterations
  if (iters == 0) {
    return 0ULL;
  }

  // FNV-1a 64-bit parameters
  const uint64_t offset_basis = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;

  // If no bins, hashing loop is empty but hash must be reset to offset basis
  if (bins == 0) {
    return offset_basis;
  }

  // Dense histogram over [0, bins)
  std::vector<uint32_t> counts(bins, 0u);
  for (uint32_t v : values) {
    if (v < bins) {
      ++counts[v];
    }
  }

  // Compute checksum (hash) once; repeated iterations would yield the same result
  uint64_t hash = offset_basis;
  for (uint32_t b = 0; b < bins; ++b) {
    hash ^= static_cast<uint64_t>(counts[b]);
    hash *= fnv_prime;
  }

  return hash;
}
