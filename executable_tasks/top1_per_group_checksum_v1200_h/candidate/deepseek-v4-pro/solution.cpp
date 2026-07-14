#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  std::vector<uint32_t> best(group_count, 0);
  const std::size_t N = groups.size();
  for (std::size_t i = 0; i < N; ++i) {
    uint32_t g = groups[i];
    uint32_t v = values[i];
    if (v > best[g]) best[g] = v;
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t g = 0; g < group_count; ++g) {
    hash = mix(hash, best[g]);
  }
  return hash;
}