#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t checksum_levels(const std::vector<int32_t>& levels) {
  uint64_t hash = 1469598103934665603ULL;
  for (int32_t level : levels) {
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(level + 1));
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
  if (iters == 0) return 0ULL;

  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  // Standard BFS using a flat queue; levels also act as visited markers.
  std::vector<int32_t> levels(n, -1);
  if (source < n) {
    levels[source] = 0;
    std::vector<uint32_t> queue;
    queue.resize(n);
    uint32_t head = 0, tail = 0;
    queue[tail++] = source;

    const uint32_t* __restrict off = offsets.data();
    const uint32_t* __restrict edg = edges.data();

    while (head < tail) {
      uint32_t u = queue[head++];
      int32_t next_level = levels[u] + 1;
      uint32_t start = off[u];
      uint32_t end = off[u + 1u];
      for (uint32_t idx = start; idx < end; ++idx) {
        uint32_t v = edg[idx];
        if (levels[v] == -1) {
          levels[v] = next_level;
          queue[tail++] = v;
        }
      }
    }
  }

  uint64_t hash = checksum_levels(levels);
  return hash;
}
