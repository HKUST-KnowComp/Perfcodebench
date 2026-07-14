#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

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
  
  const uint32_t bitmap_size = (n + 63u) / 64u;
  std::vector<uint64_t> visited(bitmap_size);
  
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next;
  frontier.reserve(n / 4);
  next.reserve(n / 4);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(levels.data(), 0xFF, n * sizeof(int32_t));
    std::memset(visited.data(), 0, bitmap_size * sizeof(uint64_t));
    
    frontier.clear();
    frontier.push_back(source);
    levels[source] = 0;
    visited[source >> 6] |= (1ULL << (source & 63));
    
    int32_t depth = 0;
    
    while (!frontier.empty()) {
      next.clear();
      ++depth;
      
      for (uint32_t u : frontier) {
        const uint32_t start = offsets[u];
        const uint32_t end = offsets[u + 1u];
        
        for (uint32_t idx = start; idx < end; ++idx) {
          const uint32_t v = edges[idx];
          const uint32_t word_idx = v >> 6;
          const uint64_t bit_mask = 1ULL << (v & 63);
          
          if ((visited[word_idx] & bit_mask) == 0) {
            visited[word_idx] |= bit_mask;
            levels[v] = depth;
            next.push_back(v);
          }
        }
      }
      
      frontier.swap(next);
    }
    
    hash = checksum_levels(levels);
  }
  
  return hash;
}
