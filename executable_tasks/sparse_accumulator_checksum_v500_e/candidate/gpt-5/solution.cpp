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
  // Preserve baseline semantics for non-positive iters
  if (iters <= 0) {
    return 0ULL;
  }

  // If domain is zero, the hash stays at the FNV offset basis per baseline
  if (domain == 0u) {
    return 1469598103934665603ULL;
  }

  // Accumulate once into a dense array over the bounded domain
  std::vector<uint64_t> accum(domain, 0ULL);
  const size_t n = indices.size();
  const uint32_t* idxp = indices.data();
  const uint32_t* valp = values.data();
  for (size_t i = 0; i < n; ++i) {
    uint32_t idx = idxp[i];
    if (idx < domain) {
      accum[idx] += static_cast<uint64_t>(valp[i]);
    }
  }

  // Compute hash over the full domain in order
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t* ap = accum.data();
  for (uint32_t k = 0; k < domain; ++k) {
    hash = mix(hash, ap[k]);
  }

  return hash;
}
