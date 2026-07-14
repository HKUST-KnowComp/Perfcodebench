#include "interface.h"

namespace {

// FNV-1a 64-bit mixing
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
  // Preserve baseline semantics for special cases
  if (iters == 0) {
    return 0ULL;
  }
  if (domain == 0) {
    // When domain is zero and at least one iteration, baseline sets hash to offset basis
    return 1469598103934665603ULL;
  }

  // Accumulate into a dense array for the bounded domain
  std::vector<uint64_t> accum(domain, 0ULL);

  const size_t n = indices.size();
  // Assume values.size() == indices.size() as per contract; iterate once
  for (size_t i = 0; i < n; ++i) {
    uint32_t idx = indices[i];
    if (idx < domain) {
      accum[idx] += static_cast<uint64_t>(values[i]);
    }
    // Else: ignore out-of-range indices to match baseline behavior
  }

  // Compute the FNV-1a style hash across the full domain exactly once
  uint64_t hash = 1469598103934665603ULL;  // FNV offset basis
  for (uint32_t key = 0; key < domain; ++key) {
    hash = mix(hash, accum[key]);
  }

  return hash;
}
