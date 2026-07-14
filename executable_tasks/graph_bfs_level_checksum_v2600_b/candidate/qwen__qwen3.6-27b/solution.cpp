#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t bfs_level_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    uint32_t source,
    int iters) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  std::vector<int32_t> levels(n, -1);
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next_frontier;
  frontier.reserve(n);
  next_frontier.reserve(n);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    frontier.clear();
    next_frontier.clear();
    frontier.push_back(source);
    levels[source] = 0;
    int32_t depth = 0;
    while (!frontier.empty()) {
      for (uint32_t u : frontier) {
        for (uint32_t idx = offsets[u]; idx < offsets[u + 1u]; ++idx) {
          uint32_t v = edges[idx];
          if (levels[v] == -1) {
            levels[v] = depth + 1;
            next_frontier.push_back(v);
          }
        }
      }
      frontier.swap(next_frontier);
      next_frontier.clear();
      ++depth;
    }

    uint64_t h = 1469598103934665603ULL;
    for (int32_t level : levels) {
      h ^= static_cast<uint64_t>(static_cast<uint32_t>(level + 1));
      h *= 1099511628211ULL;
    }
    hash = h;
  }
  return hash;
}