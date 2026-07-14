#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

uint64_t checksum_levels(const std::vector<int32_t>& levels) {
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
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  std::vector<int32_t> levels(n);
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next_frontier;
  frontier.reserve(n);
  next_frontier.reserve(n);

  const uint32_t* off = offsets.data();
  const uint32_t* adj = edges.data();
  int32_t* lev = levels.data();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(lev, lev + n, -1);
    frontier.clear();
    frontier.push_back(source);
    lev[source] = 0;
    int32_t depth = 0;

    while (!frontier.empty()) {
      next_frontier.clear();
      int32_t next_d = depth + 1;
      for (uint32_t u : frontier) {
        uint32_t begin = off[u];
        uint32_t end = off[u + 1];
        for (uint32_t idx = begin; idx < end; ++idx) {
          uint32_t v = adj[idx];
          if (lev[v] == -1) {
            lev[v] = next_d;
            next_frontier.push_back(v);
          }
        }
      }
      frontier.swap(next_frontier);
      depth = next_d;
    }
    hash = checksum_levels(levels);
  }
  return hash;
}
