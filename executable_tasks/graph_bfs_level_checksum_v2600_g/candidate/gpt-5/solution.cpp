#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

uint64_t checksum_levels(const std::vector<int32_t>& levels) {
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  for (int32_t level : levels) {
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(level + 1));
    hash *= 1099511628211ULL; // FNV-1a 64-bit prime
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

  const uint32_t* offs = offsets.data();
  const uint32_t* adj = edges.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    queue.clear();

    if (n == 0 || source >= n) {
      hash = checksum_levels(levels);
      continue;
    }

    levels[source] = 0;
    queue.push_back(source);

    for (size_t head = 0; head < queue.size(); ++head) {
      uint32_t u = queue[head];
      int32_t next_level = levels[u] + 1;
      uint32_t begin = offs[u];
      uint32_t end = offs[u + 1u];
      for (uint32_t idx = begin; idx < end; ++idx) {
        uint32_t v = adj[idx];
        if (levels[v] == -1) {
          levels[v] = next_level;
          queue.push_back(v);
        }
      }
    }

    hash = checksum_levels(levels);
  }

  return hash;
}
