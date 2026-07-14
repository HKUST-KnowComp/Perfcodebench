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
  std::vector<int32_t> levels(n, -1);
  std::vector<uint32_t> queue;
  queue.reserve(n);

  const uint32_t* __restrict off = offsets.data();
  const uint32_t* __restrict adj = edges.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    queue.clear();

    levels[source] = 0;
    queue.push_back(source);

    size_t head = 0;
    while (head < queue.size()) {
      uint32_t u = queue[head++];
      int32_t lu = levels[u];
      uint32_t begin = off[u];
      uint32_t end = off[u + 1u];
      for (uint32_t idx = begin; idx < end; ++idx) {
        uint32_t v = adj[idx];
        if (levels[v] == -1) {
          levels[v] = lu + 1;
          queue.push_back(v);
        }
      }
    }
  }

  if (iters > 0) {
    return checksum_levels(levels);
  }
  return 0ULL;
}
