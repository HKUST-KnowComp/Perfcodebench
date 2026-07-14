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
  const std::size_t n = groups.size();
  const uint32_t* gptr = groups.data();
  const uint32_t* vptr = values.data();

  std::vector<uint32_t> best(group_count);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset maxima for this iteration
    for (uint32_t g = 0; g < group_count; ++g) {
      best[g] = 0u;
    }

    // Compute per-group maxima
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t g = gptr[i];
      const uint32_t v = vptr[i];
      if (v > best[g]) {
        best[g] = v;
      }
    }

    // Compute hash of group maxima in dense order
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, best[g]);
    }
  }

  return hash;
}
