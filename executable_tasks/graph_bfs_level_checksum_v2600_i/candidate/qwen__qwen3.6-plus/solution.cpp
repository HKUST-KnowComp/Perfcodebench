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
  std::vector<uint32_t> q;
  q.reserve(n);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    q.clear();
    q.push_back(source);
    levels[source] = 0;
    size_t head = 0;
    while (head < q.size()) {
      uint32_t u = q[head++];
      int32_t d = levels[u];
      uint32_t start = offsets[u];
      uint32_t end = offsets[u + 1];
      for (uint32_t idx = start; idx < end; ++idx) {
        uint32_t v = edges[idx];
        if (levels[v] == -1) {
          levels[v] = d + 1;
          q.push_back(v);
        }
      }
    }
    hash = checksum_levels(levels);
  }
  return hash;
}