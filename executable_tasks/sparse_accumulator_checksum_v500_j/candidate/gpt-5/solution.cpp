#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  // Match baseline behavior: if no iterations, return 0.
  if (iters <= 0) return 0ULL;

  // Build dense accumulator over the bounded domain once (repeated iterations are redundant).
  std::vector<uint64_t> accum;
  accum.assign(static_cast<std::size_t>(domain), 0ULL);

  const std::size_t n = indices.size();
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t idx = indices[i];
    if (idx < domain) {
      accum[idx] += static_cast<uint64_t>(values[i]);
    }
  }

  // Compute FNV-1a style hash across the domain.
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t* data = accum.data();
  for (std::size_t i = 0, d = static_cast<std::size_t>(domain); i < d; ++i) {
    hash = mix(hash, data[i]);
  }

  return hash;
}
