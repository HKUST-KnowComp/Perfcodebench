#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bfs_level_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    uint32_t source,
    int /* iters */) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  std::vector<int32_t> levels(n, -1);
  std::vector<uint32_t> queue;
  queue.reserve(n);
  queue.push_back(source);
  levels[source] = 0;
  uint32_t frontIdx = 0;
  while (frontIdx < queue.size()) {
    uint32_t u = queue[frontIdx++];
    for (uint32_t idx = offsets[u]; idx < offsets[u + 1u]; ++idx) {
      uint32_t v = edges[idx];
      if (levels[v] == -1) {
        levels[v] = levels[u] + 1;
        queue.push_back(v);
      }
    }
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t i = 0; i < n; ++i) {
    int32_t l = levels[i];
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(l + 1));
    hash *= 1099511628211ULL;
  }
  return hash;
}
