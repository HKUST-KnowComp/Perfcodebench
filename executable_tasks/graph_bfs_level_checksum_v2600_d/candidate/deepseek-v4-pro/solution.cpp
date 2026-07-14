#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

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
  if (iters <= 0) return 0;
  
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1);
  std::vector<int32_t> levels(n, -1);
  std::vector<uint32_t> frontier;
  frontier.reserve(n);
  
  levels[source] = 0;
  frontier.push_back(source);
  size_t head = 0;
  
  while (head < frontier.size()) {
    uint32_t u = frontier[head++];
    int32_t depth_u = levels[u];
    uint32_t start = offsets[u];
    uint32_t end = offsets[u + 1];
    for (uint32_t idx = start; idx < end; ++idx) {
      uint32_t v = edges[idx];
      if (levels[v] == -1) {
        levels[v] = depth_u + 1;
        frontier.push_back(v);
      }
    }
  }
  
  return checksum_levels(levels);
}
