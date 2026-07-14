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
  const std::size_t n = groups.size();

  // Use a dense array for per-group maxima; initialized once.
  std::vector<uint32_t> best(group_count, 0u);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Accumulate per-group maxima
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t g = groups[i];
      uint32_t v = values[i];
      uint32_t cur = best[g];
      if (v > cur) best[g] = v;
    }

    // Compute hash while resetting best for the next iteration
    uint64_t h = 1469598103934665603ULL;  // FNV offset basis
    for (uint32_t g = 0; g < group_count; ++g) {
      h = mix(h, best[g]);
      best[g] = 0u;
    }
    hash = h;
  }

  return hash;
}
