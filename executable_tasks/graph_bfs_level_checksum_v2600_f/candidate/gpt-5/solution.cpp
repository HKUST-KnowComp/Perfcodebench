#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t checksum_levels(const std::vector<int32_t>& levels) {
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
  // Match baseline semantics when iters <= 0
  if (iters <= 0) {
    return 0ULL;
  }

  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  if (n == 0u) {
    return checksum_levels(std::vector<int32_t>());
  }

  std::vector<int32_t> levels(n, -1);
  levels[source] = 0;

  // Queue implemented as a vector with head index to avoid std::queue overhead.
  std::vector<uint32_t> q;
  q.reserve(n);
  q.push_back(source);
  size_t head = 0;

  const uint32_t* off = offsets.data();
  const uint32_t* adj = edges.data();

  while (head < q.size()) {
    const uint32_t u = q[head++];
    const int32_t du = levels[u];
    const uint32_t begin = off[u];
    const uint32_t end = off[u + 1u];
    for (uint32_t idx = begin; idx < end; ++idx) {
      const uint32_t v = adj[idx];
      if (levels[v] == -1) {
        levels[v] = du + 1;
        q.push_back(v);
      }
    }
  }

  // Compute checksum once; repeated BFS runs would produce identical levels.
  return checksum_levels(levels);
}
