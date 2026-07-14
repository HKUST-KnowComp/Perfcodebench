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
  
  // Use two arrays for double-buffered frontier
  std::vector<uint32_t> frontier_buf(n);
  std::vector<uint32_t> next_buf(n);
  uint32_t* frontier = frontier_buf.data();
  uint32_t* next_frontier = next_buf.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(lev_ptr, -1, n * sizeof(int32_t));
    
    frontier[0] = source;
    uint32_t frontier_size = 1;
    lev_ptr[source] = 0;
    int32_t depth = 0;
    
    while (frontier_size > 0) {
      uint32_t next_size = 0;
      depth++;
      
      for (uint32_t i = 0; i < frontier_size; ++i) {
        uint32_t u = frontier[i];
        uint32_t start = off_ptr[u];
        uint32_t end = off_ptr[u + 1];
        
        for (uint32_t idx = start; idx < end; ++idx) {
          uint32_t v = edge_ptr[idx];
          if (lev_ptr[v] == -1) {
            lev_ptr[v] = depth;
            next_frontier[next_size++] = v;
          }
        }
      }
      
      // Swap frontiers
      uint32_t* tmp = frontier;
      frontier = next_frontier;
      next_frontier = tmp;
      frontier_size = next_size;
    }
    
    hash = checksum_levels(lev_ptr, n);
  }
  
  return hash;
}