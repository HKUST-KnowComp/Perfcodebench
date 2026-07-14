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

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  // Preserve baseline behavior: if no iterations, return initial hash (which remained 0).
  if (iters <= 0) {
    return 0ULL;
  }

  // Compute per-group maxima once.
  std::vector<uint32_t> best(group_count);  // value-initialized to 0

  const std::size_t n = groups.size();
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t g = groups[i];
    const uint32_t v = values[i];
    if (g < group_count) {
      uint32_t cur = best[g];
      if (v > cur) best[g] = v;
    }
  }

  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t g = 0; g < group_count; ++g) {
    hash = mix(hash, static_cast<uint64_t>(best[g]));
  }

  return hash;
}
