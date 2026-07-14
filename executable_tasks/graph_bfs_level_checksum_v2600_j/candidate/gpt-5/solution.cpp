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
  queue.reserve(n ? std::min<uint32_t>(n, 1024u) : 0u);

  const uint32_t* __restrict o = offsets.data();
  const uint32_t* __restrict e = edges.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    queue.clear();

    levels[source] = 0;
    queue.push_back(source);

    for (size_t head = 0; head < queue.size(); ++head) {
      const uint32_t u = queue[head];
      const int32_t du = levels[u] + 1;
      const uint32_t beg = o[u];
      const uint32_t end = o[u + 1u];
      for (uint32_t idx = beg; idx < end; ++idx) {
        const uint32_t v = e[idx];
        if (levels[v] == -1) {
          levels[v] = du;
          queue.push_back(v);
        }
      }
    }

    hash = checksum_levels(levels);
  }
  return hash;
}
