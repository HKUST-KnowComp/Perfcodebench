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
  std::vector<int32_t> levels(n);
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next_frontier;
  
  // Pre-allocate to avoid reallocations during iterations
  frontier.reserve(n);
  next_frontier.reserve(n);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    
    frontier.clear();
    frontier.push_back(source);
    levels[source] = 0;
    
    int32_t depth = 0;
    while (!frontier.empty()) {
      next_frontier.clear();
      for (uint32_t u : frontier) {
        const uint32_t start = offsets[u];
        const uint32_t end = offsets[u + 1];
        for (uint32_t idx = start; idx < end; ++idx) {
          uint32_t v = edges[idx];
          if (levels[v] == -1) {
            levels[v] = depth + 1;
            next_frontier.push_back(v);
          }
        }
      }
      
      // To handle duplicates in next_frontier (multiple parents reaching same node in same level),
      // we must ensure each node is added to next_frontier only once.
      // However, the standard BFS 'if (levels[v] == -1)' check with immediate assignment 
      // 'levels[v] = depth + 1' already prevents duplicates from being added to next_frontier.
      
      frontier.swap(next_frontier);
      ++depth;
    }
    hash = checksum_levels(levels);
  }
  return hash;
}