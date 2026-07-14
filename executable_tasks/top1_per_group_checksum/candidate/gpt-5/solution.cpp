#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

constexpr uint64_t kFnv64Offset = 1469598103934665603ULL;

}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  // Dense array of maxima per group; zero-initialized.
  std::vector<uint32_t> best(group_count, 0u);

  const std::size_t n = groups.size();
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t g = groups[i];
    if (g < group_count) {
      const uint32_t v = values[i];
      uint32_t& slot = best[g];
      if (v > slot) {
        slot = v;
      }
    }
  }

  uint64_t hash = kFnv64Offset;
  for (uint32_t g = 0; g < group_count; ++g) {
    hash = mix(hash, best[g]);
  }

  return hash;
}
