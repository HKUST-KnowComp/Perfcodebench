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
    int iters) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  std::vector<int32_t> levels(n, -1);
  std::vector<uint32_t> queue(n);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    std::size_t head = 0;
    std::size_t tail = 0;
    queue[tail++] = source;
    levels[source] = 0;
    while (head < tail) {
      const uint32_t u = queue[head++];
      const int32_t next_level = levels[u] + 1;
      for (uint32_t idx = offsets[u]; idx < offsets[u + 1u]; ++idx) {
        const uint32_t v = edges[idx];
        if (levels[v] == -1) {
          levels[v] = next_level;
          queue[tail++] = v;
        }
      }
    }
    hash = checksum_levels(levels);
  }
  return hash;
}
