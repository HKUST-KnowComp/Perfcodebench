#include "interface.h"

namespace {

static inline uint64_t fnv_mix(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL; // FNV prime
  return h;
}

}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  // FNV-1a offset basis
  const uint64_t fnv_basis = 1469598103934665603ULL;

  if (group_count == 0u) {
    return fnv_basis;
  }

  // Use dense array for maxima per group; initialize to 0 for groups with no elements.
  std::vector<uint32_t> best(group_count, 0u);

  const size_t n = groups.size();
  // Build maxima per group; ignore out-of-range group indices to match baseline behavior
  for (size_t i = 0; i < n; ++i) {
    const uint32_t g = groups[i];
    if (g < group_count) {
      const uint32_t v = values[i];
      if (v > best[g]) best[g] = v;
    }
  }

  // Compute hash over all groups, mixing in 0 for groups with no elements
  uint64_t h = fnv_basis;
  for (uint32_t g = 0; g < group_count; ++g) {
    h = fnv_mix(h, static_cast<uint64_t>(best[g]));
  }

  return h;
}
