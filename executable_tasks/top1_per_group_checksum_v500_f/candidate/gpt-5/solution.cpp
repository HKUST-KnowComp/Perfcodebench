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
    int /*iters*/) {
  // Use a dense array for per-group maxima; value-initialized to zero.
  std::vector<uint32_t> best(group_count);

  const std::size_t n = groups.size();
  const uint32_t* g = groups.data();
  const uint32_t* v = values.data();
  uint32_t* b = best.data();

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t gi = g[i];
    const uint32_t vi = v[i];
    uint32_t& slot = b[gi];
    if (vi > slot) slot = vi;
  }

  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t gidx = 0; gidx < group_count; ++gidx) {
    hash = mix(hash, static_cast<uint64_t>(b[gidx]));
  }

  return hash;
}
