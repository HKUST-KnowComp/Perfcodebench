#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

uint64_t bfs_level_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    uint32_t source,
    int iters) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  std::vector<int32_t> levels(n);
  std::vector<uint32_t> queue(n);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    
    uint32_t head = 0;
    uint32_t tail = 0;

    levels[source] = 0;
    queue[tail++] = source;

    while (head < tail) {
      uint32_t u = queue[head++];
      int32_t next_level = levels[u] + 1;

      uint32_t start = offsets[u];
      uint32_t end = offsets[u + 1];
      for (uint32_t i = start; i < end; ++i) {
        uint32_t v = edges[i];
        if (levels[v] == -1) {
          levels[v] = next_level;
          queue[tail++] = v;
        }
      }
    }

    uint64_t current_hash = 1469598103934665603ULL;
    for (int32_t level : levels) {
      current_hash ^= static_cast<uint64_t>(static_cast<uint32_t>(level + 1));
      current_hash *= 1099511628211ULL;
    }
    hash = current_hash;
  }
  return hash;
}