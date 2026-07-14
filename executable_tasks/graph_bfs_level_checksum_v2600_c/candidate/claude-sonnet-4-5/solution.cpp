#include "interface.h"

#include <cstdint>
#include <cstring>
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
  std::vector<uint32_t> frontier_a;
  std::vector<uint32_t> frontier_b;
  std::vector<uint8_t> visited(n);
  
  frontier_a.reserve(n);
  frontier_b.reserve(n);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(levels.data(), 0xFF, n * sizeof(int32_t));
    std::memset(visited.data(), 0, n);
    
    frontier_a.clear();
    frontier_a.push_back(source);
    levels[source] = 0;
    visited[source] = 1;
    
    std::vector<uint32_t>* current = &frontier_a;
    std::vector<uint32_t>* next = &frontier_b;
    
    int32_t depth = 0;
    
    while (!current->empty()) {
      next->clear();
      ++depth;
      
      for (uint32_t u : *current) {
        const uint32_t start = offsets[u];
        const uint32_t end = offsets[u + 1u];
        
        for (uint32_t idx = start; idx < end; ++idx) {
          const uint32_t v = edges[idx];
          if (!visited[v]) {
            visited[v] = 1;
            levels[v] = depth;
            next->push_back(v);
          }
        }
      }
      
      std::swap(current, next);
    }
    
    hash = checksum_levels(levels);
  }
  
  return hash;
}
