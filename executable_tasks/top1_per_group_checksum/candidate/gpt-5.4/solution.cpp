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
  if (values.size() != n) {
    return 0;
  }

  std::vector<uint32_t> best(group_count, 0);
  std::vector<uint32_t> touched;
  touched.reserve(n < static_cast<std::size_t>(group_count) ? n : static_cast<std::size_t>(group_count));
  std::vector<uint8_t> seen(group_count, 0);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    touched.clear();

    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t g = groups[i];
      const uint32_t v = values[i];

      if (!seen[g]) {
        seen[g] = 1;
        best[g] = v;
        touched.push_back(g);
      } else if (v > best[g]) {
        best[g] = v;
      }
    }

    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, best[g]);
    }

    for (uint32_t g : touched) {
      seen[g] = 0;
      best[g] = 0;
    }
  }
  return hash;
}
