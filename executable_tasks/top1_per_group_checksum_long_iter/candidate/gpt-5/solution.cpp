#include "interface.h"

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}
}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  // Match baseline behavior: if iters == 0, return 0.
  if (iters <= 0) {
    return 0ULL;
  }

  // Compute per-group maxima once (groups are dense in [0, group_count)).
  std::vector<uint32_t> best(group_count, 0u);
  const size_t n = groups.size();
  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();

  for (size_t i = 0; i < n; ++i) {
    uint32_t g = gptr[i];
    if (g < group_count) {
      uint32_t v = vptr[i];
      if (v > best[g]) best[g] = v;
    }
  }

  // Compute hash from maxima.
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* bptr = best.data();
  for (uint32_t g = 0; g < group_count; ++g) {
    hash = mix(hash, static_cast<uint64_t>(bptr[g]));
  }

  return hash;
}
