#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

uint64_t checksum_levels(const int32_t* levels, uint32_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t i = 0; i < n; ++i) {
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
  std::vector<int32_t> levels(n, -1);
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next;
  frontier.reserve(n);
  next.reserve(n);

  const uint32_t* offsets_ptr = offsets.data();
  const uint32_t* edges_ptr = edges.data();
  int32_t* levels_ptr = levels.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels_ptr, levels_ptr + n, -1);
    frontier.clear();
    frontier.push_back(source);
    levels_ptr[source] = 0;
    int32_t depth = 0;
    
    while (!frontier.empty()) {
      next.clear();
      for (uint32_t u : frontier) {
        uint32_t start = offsets_ptr[u];
        uint32_t end = offsets_ptr[u + 1];
        for (uint32_t idx = start; idx < end; ++idx) {
          uint32_t v = edges_ptr[idx];
          if (levels_ptr[v] == -1) {
            levels_ptr[v] = depth + 1;
            next.push_back(v);
          }
        }
      }
      frontier.swap(next);
      ++depth;
    }
    hash = checksum_levels(levels_ptr, n);
  }
  return hash;
}