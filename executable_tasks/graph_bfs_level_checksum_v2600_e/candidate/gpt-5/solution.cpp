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

  const uint32_t* offsets_data = offsets.data();
  const uint32_t* edges_data = edges.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    queue.clear();

    levels[source] = 0;
    queue.push_back(source);

    size_t head = 0;
    while (head < queue.size()) {
      uint32_t u = queue[head++];
      int32_t du = levels[u];
      const uint32_t start = offsets_data[u];
      const uint32_t end = offsets_data[u + 1u];
      for (uint32_t idx = start; idx < end; ++idx) {
        uint32_t v = edges_data[idx];
        if (levels[v] == -1) {
          levels[v] = du + 1;
          queue.push_back(v);
        }
      }
    }

    hash = checksum_levels(levels);
  }

  return hash;
}
