#include "interface.h"

#include <cstdint>
#include <vector>
#include <queue>

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
  std::vector<int32_t> levels(n, -1);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    std::queue<uint32_t> frontier;
    frontier.push(source);
    levels[source] = 0;
    while (!frontier.empty()) {
      uint32_t u = frontier.front();
      frontier.pop();
      for (uint32_t idx = offsets[u]; idx < offsets[u + 1u]; ++idx) {
        uint32_t v = edges[idx];
        if (levels[v] == -1) {
          levels[v] = levels[u] + 1;
          frontier.push(v);
        }
      }
    }
    hash = checksum_levels(levels);
  }
  return hash;
}
