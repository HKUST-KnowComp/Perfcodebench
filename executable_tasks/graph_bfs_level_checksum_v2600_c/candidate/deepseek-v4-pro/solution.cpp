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
    uint32_t cur = 0;
    int32_t depth = 0;
    while (cur < q.size()) {
      uint32_t limit = q.size();
      while (cur < limit) {
        uint32_t u = q[cur];
        const auto off_end = offsets[u + 1];
        for (uint32_t idx = offsets[u]; idx < off_end; ++idx) {
          uint32_t v = edges[idx];
          if (levels[v] == -1) {
            levels[v] = depth + 1;
            q.push_back(v);
          }
        }
        ++cur;
      }
      ++depth;
    }
    hash = checksum_levels(levels);
  }
  return hash;
}
