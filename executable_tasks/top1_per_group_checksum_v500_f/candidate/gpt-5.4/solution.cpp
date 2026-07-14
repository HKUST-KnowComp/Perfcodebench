#include "interface.h"

#include <cstdint>
#include <vector>

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

  // Dense groups: use flat arrays instead of a hash table.
  std::vector<uint32_t> best(group_count, 0);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset maxima for this iteration.
    if (group_count != 0) {
      best.assign(group_count, 0);
    }

    const uint32_t* gptr = groups.data();
    const uint32_t* vptr = values.data();
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t g = gptr[i];
      const uint32_t v = vptr[i];
      uint32_t& cur = best[g];
      if (v > cur) cur = v;
    }

    uint64_t h = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      h = mix(h, best[g]);
    }
    hash = h;
  }

  return hash;
}
