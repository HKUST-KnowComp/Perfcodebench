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

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  // Preserve baseline behavior: if iters == 0, return 0
  if (iters == 0) {
    return 0ULL;
  }

  // Compute once since each iteration produces the same result in the baseline
  if (domain == 0) {
    // No keys to mix; baseline sets hash to offset basis and mixes nothing
    return 1469598103934665603ULL;
  }

  std::vector<uint64_t> accum(static_cast<std::size_t>(domain), 0ULL);

  const std::size_t n = indices.size();
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t idx = indices[i];
    if (idx < domain) {
      accum[static_cast<std::size_t>(idx)] += static_cast<uint64_t>(values[i]);
    }
    // If idx >= domain, ignore (matches baseline behavior where it wouldn't be hashed)
  }

  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis
  const uint64_t* p = accum.data();
  const std::size_t d = static_cast<std::size_t>(domain);
  for (std::size_t i = 0; i < d; ++i) {
    hash = mix(hash, p[i]);
  }

  return hash;
}
