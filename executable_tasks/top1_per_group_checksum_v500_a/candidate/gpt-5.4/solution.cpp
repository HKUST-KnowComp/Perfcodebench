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
  if (n == 0) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, 0u);
    }
    return hash;
  }

  std::vector<uint32_t> best(group_count, 0);
  std::vector<uint32_t> seen(group_count, 0);
  uint32_t epoch = 1;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    if (epoch == 0) {
      for (uint32_t g = 0; g < group_count; ++g) {
        seen[g] = 0;
      }
      epoch = 1;
    }

    const uint32_t* gptr = groups.data();
    const uint32_t* vptr = values.data();
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t g = gptr[i];
      const uint32_t v = vptr[i];
      if (seen[g] != epoch) {
        seen[g] = epoch;
        best[g] = v;
      } else if (best[g] < v) {
        best[g] = v;
      }
    }

    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, seen[g] == epoch ? best[g] : 0u);
    }

    ++epoch;
  }

  return hash;
}
