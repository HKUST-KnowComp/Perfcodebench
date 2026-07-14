#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t checksum_levels_stamped(const std::vector<int32_t>& levels,
                                        const std::vector<uint32_t>& stamp,
                                        uint32_t iter_id) {
  // FNV-1a 64-bit
  uint64_t hash = 1469598103934665603ULL;
  const size_t n = levels.size();
  for (size_t i = 0; i < n; ++i) {
    int32_t lvl = (stamp[i] == iter_id) ? levels[i] : -1;
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
  if (n == 0u) {
    return 1469598103934665603ULL ^ 1099511628211ULL; // hash of empty? Not used; n==0 unlikely
  }

  std::vector<int32_t> level_val(n, -1);
  std::vector<uint32_t> stamp(n, 0u);
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next;
  frontier.reserve(n);
  next.reserve(n);

  const uint32_t* __restrict off = offsets.data();
  const uint32_t* __restrict nbr = edges.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t iter_id = static_cast<uint32_t>(iter + 1);

    frontier.clear();
    next.clear();

    // Initialize source
    stamp[source] = iter_id;
    level_val[source] = 0;
    frontier.push_back(source);

    int32_t depth = 0;
    while (!frontier.empty()) {
      next.clear();
      const int32_t next_depth = depth + 1;

      // Expand current frontier
      const size_t fsz = frontier.size();
      for (size_t fi = 0; fi < fsz; ++fi) {
        const uint32_t u = frontier[fi];
        const uint32_t beg = off[u];
        const uint32_t end = off[u + 1u];
        for (uint32_t ei = beg; ei < end; ++ei) {
          const uint32_t v = nbr[ei];
          if (stamp[v] != iter_id) {
            stamp[v] = iter_id;
            level_val[v] = next_depth;
            next.push_back(v);
          }
        }
      }

      frontier.swap(next);
      ++depth;
    }

    hash = checksum_levels_stamped(level_val, stamp, iter_id);
  }

  return hash;
}
