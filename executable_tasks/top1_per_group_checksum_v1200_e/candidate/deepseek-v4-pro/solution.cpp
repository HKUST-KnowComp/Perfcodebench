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
  if (iters <= 0) return 0;

  // Reuse thread-local storage for best per group
  static thread_local std::vector<uint32_t> best;
  best.assign(group_count, 0);

  const size_t n = groups.size();
  for (size_t i = 0; i < n; ++i) {
    const uint32_t g = groups[i];
    const uint32_t v = values[i];
    if (v > best[g]) best[g] = v;
  }

  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t g = 0; g < group_count; ++g) {
    hash = mix(hash, best[g]);
  }
  return hash;
}
