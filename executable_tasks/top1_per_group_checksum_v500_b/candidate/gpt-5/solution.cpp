#include "interface.h"

#include <cstddef>

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  // Use a dense array for per-group maxima (groups are dense [0, group_count)).
  std::vector<uint32_t> best(group_count, 0u);

  const std::size_t n = groups.size();
  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t gid = gptr[i];
    const uint32_t val = vptr[i];
    uint32_t& slot = best[gid];
    if (val > slot) slot = val;
  }

  // FNV-1a style hashing over group maxima in order [0, group_count)
  uint64_t hash = 1469598103934665603ULL; // FNV offset basis
  for (uint32_t g = 0; g < group_count; ++g) {
    hash ^= static_cast<uint64_t>(best[g]);
    hash *= 1099511628211ULL; // FNV prime
  }

  return hash;
}
