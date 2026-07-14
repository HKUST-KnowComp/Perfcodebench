#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>
#include <queue>

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
  frontier.reserve(n);
  next_frontier.reserve(n);
  
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    frontier.clear();
    
    levels[source] = 0;
    frontier.push_back(source);
    
    size_t head = 0;
    while (head < frontier.size()) {
      // We use a level-by-level approach to match the original logic's structure
      // but without the expensive sort/unique.
      // However, a standard BFS queue is equivalent for level assignment.
      // To strictly follow the 'depth' logic of the original:
      
      // The original code processed all nodes in 'frontier' to find 'candidates',
      // then unique-d them, then checked levels.
      // This is equivalent to a standard BFS where we only add unvisited neighbors.
      
      // Let's implement a standard efficient BFS.
      // The original code's 'depth' increments after each frontier swap.
      // This is exactly what a level-order traversal does.
      
      // Re-implementing the level-order logic efficiently:
      // We'll use the 'frontier' as the current level and 'next_frontier' as the next.
      // This is already what the loop structure below does.
      break; 
    }
    
    // Standard BFS implementation
    frontier.clear();
    frontier.push_back(source);
    levels[source] = 0;
    
    size_t current_level_start = 0;
    while (current_level_start < frontier.size()) {
      size_t current_level_end = frontier.size();
      // The original code processes the whole frontier to find the next level
      // We can just iterate through the current frontier.
      for (size_t i = current_level_start; i < current_level_end; ++i) {
        uint32_t u = frontier[i];
        uint32_t start_idx = offsets[u];
        uint32_t end_idx = offsets[u + 1];
        for (uint32_t idx = start_idx; idx < end_idx; ++idx) {
          uint32_t v = edges[idx];
          if (levels[v] == -1) {
            levels[v] = levels[u] + 1;
            frontier.push_back(v);
          }
        }
      }
      current_level_start = current_level_end;
    }
    
    final_hash = checksum_levels(levels);
  }
  
  return final_hash;
}
