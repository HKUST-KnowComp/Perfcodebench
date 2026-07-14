#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t checksum_levels(const int32_t* levels, uint32_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(levels[i] + 1));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t bfs_level_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    uint32_t source,
    int iters) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() > 0 ? offsets.size() - 1u : 0u);
  if (iters <= 0) return 0;
  if (n == 0) return 1469598103934665603ULL;

  std::vector<int32_t> levels(n);
  std::vector<uint32_t> queue(n);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (uint32_t i = 0; i < n; ++i) levels[i] = -1;

    if (source >= n) {
      hash = checksum_levels(levels.data(), n);
      continue;
    }

    uint32_t head = 0;
    uint32_t tail = 0;
    queue[tail++] = source;
    levels[source] = 0;

    while (head < tail) {
      const uint32_t u = queue[head++];
      const int32_t next_level = levels[u] + 1;
      const uint32_t begin = offsets[u];
      const uint32_t end = offsets[u + 1u];
      for (uint32_t idx = begin; idx < end; ++idx) {
        const uint32_t v = edges[idx];
        if (levels[v] == -1) {
          levels[v] = next_level;
          queue[tail++] = v;
        }
      }
    }

    hash = checksum_levels(levels.data(), n);
  }

  return hash;
}
