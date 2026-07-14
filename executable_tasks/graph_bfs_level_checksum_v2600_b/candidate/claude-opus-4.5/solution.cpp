#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

namespace {

inline uint64_t checksum_levels(const int32_t* levels, uint32_t n) {
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
  if (n == 0) return 1469598103934665603ULL;
  
  const uint32_t* __restrict off_ptr = offsets.data();
  const uint32_t* __restrict edge_ptr = edges.data();
  
  std::vector<int32_t> levels(n);
  int32_t* __restrict lev_ptr = levels.data();
  
  // Use two frontiers and swap
  std::vector<uint32_t> frontier_a, frontier_b;
  frontier_a.reserve(n);
  frontier_b.reserve(n);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(lev_ptr, 0xFF, n * sizeof(int32_t)); // -1
    
    frontier_a.clear();
    frontier_a.push_back(source);
    lev_ptr[source] = 0;
    
    std::vector<uint32_t>* curr = &frontier_a;
    std::vector<uint32_t>* next = &frontier_b;
    
    int32_t depth = 0;
    
    while (!curr->empty()) {
      next->clear();
      const int32_t next_depth = depth + 1;
      
      for (uint32_t u : *curr) {
        const uint32_t start = off_ptr[u];
        const uint32_t end = off_ptr[u + 1];
        
        for (uint32_t idx = start; idx < end; ++idx) {
          const uint32_t v = edge_ptr[idx];
          if (lev_ptr[v] == -1) {
            lev_ptr[v] = next_depth;
            next->push_back(v);
          }
        }
      }
      
      std::swap(curr, next);
      ++depth;
    }
    
    hash = checksum_levels(lev_ptr, n);
  }
  
  return hash;
}