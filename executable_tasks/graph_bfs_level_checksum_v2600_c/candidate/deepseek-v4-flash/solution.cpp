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
  std::vector<uint32_t> gen(n, 0);
  std::vector<uint32_t> queue(n);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint32_t currentGen = iter + 1;
    uint32_t head = 0, tail = 0;
    queue[tail++] = source;
    gen[source] = currentGen;
    levels[source] = 0;

    while (head < tail) {
      uint32_t u = queue[head++];
      uint32_t start = offsets[u];
      uint32_t end = offsets[u + 1u];
      int32_t nextLevel = levels[u] + 1;
      for (uint32_t idx = start; idx < end; ++idx) {
        uint32_t v = edges[idx];
        if (gen[v] != currentGen) {
          gen[v] = currentGen;
          levels[v] = nextLevel;
          queue[tail++] = v;
        }
      }
    }
    hash = checksum_levels(levels);
  }
  return hash;
}
