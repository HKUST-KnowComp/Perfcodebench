#include "interface.h"

#include <cstring>

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
  const size_t n = groups.size();
  // Preallocate maxima buffer for all groups
  std::vector<uint32_t> best(group_count);

  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset maxima to 0 for this iteration
    if (group_count) {
      std::memset(best.data(), 0, static_cast<size_t>(group_count) * sizeof(uint32_t));
    }

    // Accumulate per-group maxima
    const uint32_t* g = gptr;
    const uint32_t* v = vptr;
    for (size_t i = 0; i < n; ++i) {
      uint32_t gi = g[i];
      if (gi < group_count) {
        uint32_t vi = v[i];
        uint32_t old = best[gi];
        best[gi] = (vi > old) ? vi : old;
      }
    }

    // Compute hash of maxima using FNV-1a style mix, over all groups
    uint64_t h = 1469598103934665603ULL;
    const uint32_t* b = best.data();
    for (uint32_t gi = 0; gi < group_count; ++gi) {
      h = mix(h, static_cast<uint64_t>(b[gi]));
    }
    hash = h;
  }

  return hash;
}
