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
        for (uint32_t i = start; i < end; ++i) {
          uint32_t v = edges[i];
          if (levels[v] == -1) {
            levels[v] = depth + 1;
            next_frontier.push_back(v);
          }
        }
      }
      
      // To handle duplicate neighbors in the same level correctly without sorting,
      // we must ensure a node isn't added to next_frontier multiple times.
      // However, the standard BFS 'if (levels[v] == -1)' check handles this 
      // if we set the level immediately upon discovery.
      // But wait, if multiple nodes in 'frontier' point to the same 'v', 
      // 'levels[v]' is set by the first one, and the second one will see 
      // levels[v] != -1 and skip it. This is correct and efficient.
      
      // There is a subtle bug in the logic above: if two nodes in 'frontier' 
      // point to the same 'v', the first one sets levels[v] = depth + 1, 
      // and the second one sees levels[v] != -1 and skips. 
      // This is actually correct for BFS.
      
      // However, we need to handle the case where multiple nodes in 'frontier' 
      // point to the same 'v' in the SAME level. The current logic:
      // 1. u1 -> v (levels[v] becomes depth+1, v added to next_frontier)
      // 2. u2 -> v (levels[v] is already depth+1, v NOT added to next_frontier)
      // This is exactly what we want.

      // Wait, there is a catch: if we set levels[v] = depth + 1 inside the loop, 
      // we must ensure we don't add the same 'v' to next_frontier multiple times.
      // The current code does: if (levels[v] == -1) { levels[v] = depth + 1; next_frontier.push_back(v); }
      // This is correct and prevents duplicates in next_frontier.

      frontier.swap(next_frontier);
      ++depth;
    }
    hash = checksum_levels(levels);
  }
  return hash;
}