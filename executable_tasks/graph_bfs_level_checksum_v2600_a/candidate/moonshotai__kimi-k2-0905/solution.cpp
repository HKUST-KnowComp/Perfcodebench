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
  frontier.reserve(n);
  std::vector<uint32_t> next;
  next.reserve(n);
  std::vector<uint8_t> in_queue(n, 0);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    std::fill(in_queue.begin(), in_queue.end(), 0);
    frontier.clear();
    frontier.push_back(source);
    levels[source] = 0;
    in_queue[source] = 1;
    int32_t depth = 0;
    while (!frontier.empty()) {
      next.clear();
      for (uint32_t u : frontier) {
        const uint32_t beg = offsets[u];
        const uint32_t end = offsets[u + 1];
        for (uint32_t idx = beg; idx < end; ++idx) {
          const uint32_t v = edges[idx];
          if (levels[v] == -1 && !in_queue[v]) {
            levels[v] = depth + 1;
            in_queue[v] = 1;
            next.push_back(v);
          }
        }
      }
      frontier.swap(next);
      ++depth;
    }
    hash = checksum_levels(levels);
  }
  return hash;
}