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
  if (iters <= 0) return 0;
  std::vector<uint32_t> best(group_count, 0);
  const uint32_t* gp = groups.data();
  const uint32_t* vp = values.data();
  const std::size_t n = groups.size();
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t g = gp[i];
    const uint32_t v = vp[i];
    if (v > best[g]) {
      best[g] = v;
    }
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t g = 0; g < group_count; ++g) {
    hash = mix(hash, best[g]);
  }
  return hash;
}