#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

uint64_t checksum_levels(const int32_t* levels, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(levels[i] + 1));
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
  std::vector<int32_t> levels(n);
  std::vector<uint32_t> queue;
  queue.reserve(n);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    queue.clear();

    queue.push_back(source);
    levels[source] = 0;
    size_t qhead = 0;

    while (qhead < queue.size()) {
      const uint32_t u = queue[qhead++];
      const int32_t next_level = levels[u] + 1;
      const uint32_t beg = offsets[u];
      const uint32_t end = offsets[u + 1];
      for (uint32_t idx = beg; idx < end; ++idx) {
        const uint32_t v = edges[idx];
        if (levels[v] == -1) {
          levels[v] = next_level;
          queue.push_back(v);
        }
      }
    }

    hash = checksum_levels(levels.data(), n);
  }
  return hash;
}
