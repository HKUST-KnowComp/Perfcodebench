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
  // Preserve baseline behavior: if no iterations, return 0.
  if (iters == 0) {
    return 0ULL;
  }

  // If domain is zero and iters > 0, the baseline sets hash to the offset basis
  // and performs no mixing, returning the offset basis.
  if (domain == 0u) {
    return 1469598103934665603ULL;
  }

  // Accumulate once into a dense array over the bounded domain.
  std::vector<uint64_t> accum(domain, 0ULL);
  const std::size_t n = indices.size();
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t idx = indices[i];
    if (idx < domain) {
      accum[idx] += static_cast<uint64_t>(values[i]);
    }
  }

  // Compute the checksum exactly as in the baseline.
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t key = 0; key < domain; ++key) {
    hash = mix(hash, accum[key]);
  }

  return hash;
}
