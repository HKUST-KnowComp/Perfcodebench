#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t checksum_levels_u32(const std::vector<uint32_t>& levels) {
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* p = levels.data();
  const uint32_t* end = p + levels.size();
  while (p != end) {
    hash ^= static_cast<uint64_t>((*p) + 1u);
    hash *= 1099511628211ULL;
    ++p;
  }
  return hash;
}

}  // namespace

uint64_t bfs_level_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    uint32_t source,
    int iters) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  if (n == 0 || source >= n || iters <= 0) {
    return 0;
  }

  const uint32_t kUnvisited = 0xFFFFFFFFu;

  std::vector<uint32_t> levels(n, kUnvisited);
  std::vector<uint32_t> queue(n);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (uint32_t i = 0; i < n; ++i) {
      levels[i] = kUnvisited;
    }

    uint32_t head = 0;
    uint32_t tail = 0;
    queue[tail++] = source;
    levels[source] = 0;

    while (head < tail) {
      const uint32_t u = queue[head++];
      const uint32_t next_level = levels[u] + 1u;
      const uint32_t begin = offsets[u];
      const uint32_t end = offsets[u + 1u];
      for (uint32_t idx = begin; idx < end; ++idx) {
        const uint32_t v = edges[idx];
        if (levels[v] == kUnvisited) {
          levels[v] = next_level;
          queue[tail++] = v;
        }
      }
    }

    hash = checksum_levels_u32(levels);
  }

  return hash;
}
