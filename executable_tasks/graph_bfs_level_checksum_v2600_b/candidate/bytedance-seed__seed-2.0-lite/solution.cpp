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
  std::vector<uint32_t> current_frontier;
  std::vector<uint32_t> next_frontier;
  // Preallocate maximum size to avoid repeated heap reallocations
  current_frontier.reserve(n);
  next_frontier.reserve(n);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    current_frontier.clear();
    next_frontier.clear();
    // Initialize source node
    levels[source] = 0;
    current_frontier.push_back(source);
    int32_t depth = 0;

    while (!current_frontier.empty()) {
      next_frontier.clear();
      for (uint32_t u : current_frontier) {
        const uint32_t start = offsets[u];
        const uint32_t end = offsets[u + 1u];
        for (uint32_t idx = start; idx < end; ++idx) {
          const uint32_t v = edges[idx];
          if (levels[v] == -1) {
            levels[v] = depth + 1;
            next_frontier.push_back(v);
          }
        }
      }
      current_frontier.swap(next_frontier);
      ++depth;
    }
    final_hash = checksum_levels(levels);
  }
  return final_hash;
}