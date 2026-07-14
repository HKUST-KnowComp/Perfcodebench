#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>
#include <cstring>

namespace {

uint64_t checksum_levels(const std::vector<int32_t>& levels) {
  uint64_t hash = 1469598103934665603ULL;
  const size_t n = levels.size();
  size_t i = 0;
  
  // Process 4 elements at a time
  for (; i + 3 < n; i += 4) {
    uint64_t v0 = static_cast<uint64_t>(static_cast<uint32_t>(levels[i] + 1));
    uint64_t v1 = static_cast<uint64_t>(static_cast<uint32_t>(levels[i+1] + 1));
    uint64_t v2 = static_cast<uint64_t>(static_cast<uint32_t>(levels[i+2] + 1));
    uint64_t v3 = static_cast<uint64_t>(static_cast<uint32_t>(levels[i+3] + 1));
    
    hash ^= v0;
    hash *= 1099511628211ULL;
    hash ^= v1;
    hash *= 1099511628211ULL;
    hash ^= v2;
    hash *= 1099511628211ULL;
    hash ^= v3;
    hash *= 1099511628211ULL;
  }
  
  // Handle remaining elements
  for (; i < n; ++i) {
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
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next;
  
  // Bitmap for visited tracking (1 bit per node)
  const size_t bitmap_size = (n + 63) / 64;
  std::vector<uint64_t> visited_bitmap(bitmap_size);
  
  // Pre-allocate with reasonable capacity
  frontier.reserve(n / 4);
  next.reserve(n / 4);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    std::memset(visited_bitmap.data(), 0, bitmap_size * sizeof(uint64_t));
    
    frontier.clear();
    frontier.push_back(source);
    levels[source] = 0;
    
    // Mark source as visited
    visited_bitmap[source >> 6] |= (1ULL << (source & 63));
    
    int32_t depth = 0;
    
    while (!frontier.empty()) {
      next.clear();
      const int32_t next_depth = depth + 1;
      
      // Process all nodes in current frontier
      for (uint32_t u : frontier) {
        const uint32_t start = offsets[u];
        const uint32_t end = offsets[u + 1u];
        
        for (uint32_t idx = start; idx < end; ++idx) {
          const uint32_t v = edges[idx];
          const uint32_t word_idx = v >> 6;
          const uint64_t bit_mask = 1ULL << (v & 63);
          
          // Check and set visited atomically using bitmap
          if ((visited_bitmap[word_idx] & bit_mask) == 0) {
            visited_bitmap[word_idx] |= bit_mask;
            levels[v] = next_depth;
            next.push_back(v);
          }
        }
      }
      
      frontier.swap(next);
      ++depth;
    }
    
    hash = checksum_levels(levels);
  }
  
  return hash;
}
