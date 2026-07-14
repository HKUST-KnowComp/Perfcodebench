#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t fnv1a_checksum_stamped(
    const std::vector<int32_t>& levels,
    const std::vector<uint32_t>& seen,
    uint32_t cur_stamp) {
  uint64_t hash = 1469598103934665603ULL;
  const size_t n = levels.size();
  for (size_t i = 0; i < n; ++i) {
    int32_t lvl = (seen[i] == cur_stamp) ? levels[i] : -1;
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(lvl + 1));
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
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  if (n == 0 || iters <= 0) {
    return 0ULL;
  }

  // Level array and visitation stamps (to avoid O(n) clears per iter)
  std::vector<int32_t> level(n);
  std::vector<uint32_t> seen(n, 0u);

  // Frontiers
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next;
  frontier.reserve(n);
  next.reserve(n);

  const uint32_t* RESTRICT off = offsets.data();
  const uint32_t* RESTRICT adj = edges.data();

  uint64_t hash = 0ULL;
  uint32_t cur_stamp = 0u;

  for (int iter = 0; iter < iters; ++iter) {
    ++cur_stamp;  // new BFS run

    frontier.clear();
    next.clear();

    // Initialize source
    seen[source] = cur_stamp;
    level[source] = 0;
    frontier.push_back(source);

    int32_t depth = 0;
    while (!frontier.empty()) {
      const int32_t next_depth = depth + 1;

      // Expand current frontier
      for (size_t i = 0, fsz = frontier.size(); i < fsz; ++i) {
        uint32_t u = frontier[i];
        uint32_t b = off[u];
        uint32_t e = off[u + 1u];
        for (uint32_t idx = b; idx < e; ++idx) {
          uint32_t v = adj[idx];
          if (seen[v] != cur_stamp) {
            seen[v] = cur_stamp;
            level[v] = next_depth;
            next.push_back(v);
          }
        }
      }

      frontier.swap(next);
      next.clear();
      depth = next_depth;
    }

    // Compute checksum for this iteration's levels without clearing the array.
    hash = fnv1a_checksum_stamped(level, seen, cur_stamp);
  }

  return hash;
}
