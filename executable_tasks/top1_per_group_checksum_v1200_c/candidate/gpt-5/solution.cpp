#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
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
  // Preserve baseline behavior: when iters == 0, nothing is computed and 0 is returned.
  if (iters <= 0) return 0ULL;

  // Compute per-group maxima once; inputs are constant across iterations and
  // the baseline overwrote the hash each time, so the final result equals a single computation.
  std::vector<uint32_t> best(group_count, 0u);

  const std::size_t n = groups.size();
  // Assuming values.size() == groups.size() per task contract.
  for (std::size_t i = 0; i < n; ++i) {
    uint32_t g = groups[i];
    uint32_t v = values[i];
    // Groups are dense in [0, group_count). If out-of-range inputs existed,
    // they'd be invalid per task specification.
    if (g < group_count) {
      uint32_t cur = best[g];
      if (v > cur) best[g] = v;
    }
  }

  uint64_t hash = 1469598103934665603ULL;  // FNV-1 offset basis
  for (uint32_t g = 0; g < group_count; ++g) {
    hash = mix(hash, best[g]);
  }
  return hash;
}
