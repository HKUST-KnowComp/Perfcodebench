#include "interface.h"

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
    int /*iters*/) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  std::vector<int32_t> levels(n, -1);
  if (n == 0 || source >= n) {
    return checksum_levels(levels);
  }

  const uint32_t* offp = offsets.data();
  const uint32_t* edgep = edges.data();
  int32_t* levelp = levels.data();

  // Single-queue BFS for efficiency
  std::vector<uint32_t> queue;
  queue.reserve(n);
  levelp[source] = 0;
  queue.push_back(source);

  size_t head = 0;
  while (head < queue.size()) {
    uint32_t u = queue[head++];
    int32_t nextLevel = levelp[u] + 1;
    uint32_t start = offp[u];
    uint32_t end = offp[u + 1u];
    for (uint32_t idx = start; idx < end; ++idx) {
      uint32_t v = edgep[idx];
      if (levelp[v] == -1) {
        levelp[v] = nextLevel;
        queue.push_back(v);
      }
    }
  }

  return checksum_levels(levels);
}
