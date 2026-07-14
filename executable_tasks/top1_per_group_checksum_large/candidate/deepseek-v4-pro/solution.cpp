#include "interface.h"

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  std::vector<uint32_t> best(group_count);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset maxima for this iteration
    for (auto& v : best) v = 0;
    for (std::size_t i = 0; i < groups.size(); ++i) {
      const uint32_t g = groups[i];
      if (g < group_count) {
        const uint32_t val = values[i];
        if (val > best[g]) {
          best[g] = val;
        }
      }
    }
    // Compute checksum over group maxima
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash ^= best[g];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}