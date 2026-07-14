#include "interface.h"

#include <vector>

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  if (iters <= 0) return 0;

  // Compute the maximum value for each group once.
  std::vector<uint32_t> maxvals(group_count, 0);
  const std::size_t n = groups.size();
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t g = groups[i];
    if (g < group_count) {
      const uint32_t v = values[i];
      if (v > maxvals[g]) maxvals[g] = v;
    }
  }

  const uint64_t kInitHash = 1469598103934665603ULL;
  auto mix = [](uint64_t h, uint64_t v) -> uint64_t {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
  };

  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = kInitHash;
    for (uint32_t g = 0; g < group_count; ++g) {
      h = mix(h, maxvals[g]);
    }
    final_hash = h;
  }
  return final_hash;
}