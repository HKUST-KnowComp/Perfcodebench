#include "interface.h"

#include <cstdint>
#include <cstring>
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
  if (n == 0) return 1469598103934665603ULL;
  
  const uint32_t m = static_cast<uint32_t>(edges.size());
  const uint32_t* __restrict off_ptr = offsets.data();
  const uint32_t* __restrict edge_ptr = edges.data();
  
  std::vector<int32_t> levels(n, -1);
  int32_t* __restrict lev_ptr = levels.data();
  
  // Bitmap for visited nodes - faster than checking levels array
  const uint32_t bitmap_size = (n + 63u) / 64u;
  std::vector<uint64_t> visited(bitmap_size, 0);
  uint64_t* __restrict vis_ptr = visited.data();
  
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next_frontier;
  frontier.reserve(n);
  next_frontier.reserve(n);
  
  uint64_t hash = 0;
  
  // Threshold for switching to bottom-up (alpha parameter)
  // When frontier edges > m/alpha, switch to bottom-up
  const uint32_t alpha_threshold = m / 14;
  // Threshold for switching back to top-down (beta parameter)
  // When frontier size < n/beta, switch to top-down
  const uint32_t beta_threshold = n / 24;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset state
    std::memset(lev_ptr, -1, n * sizeof(int32_t));
    std::memset(vis_ptr, 0, bitmap_size * sizeof(uint64_t));
    
    frontier.clear();
    frontier.push_back(source);
    lev_ptr[source] = 0;
    vis_ptr[source >> 6] |= (1ULL << (source & 63));
    
    int32_t depth = 0;
    bool use_bottom_up = false;
    uint32_t frontier_edges = off_ptr[source + 1] - off_ptr[source];
    
    while (!frontier.empty()) {
      next_frontier.clear();
      const int32_t next_depth = depth + 1;
      
      // Decide direction based on frontier size
      if (!use_bottom_up && frontier_edges > alpha_threshold) {
        use_bottom_up = true;
      } else if (use_bottom_up && frontier.size() < beta_threshold) {
        use_bottom_up = false;
      }
      
      if (use_bottom_up) {
        // Bottom-up: iterate over all unvisited nodes
        for (uint32_t v = 0; v < n; ++v) {
          if (lev_ptr[v] != -1) continue;
          
          const uint32_t start = off_ptr[v];
          const uint32_t end = off_ptr[v + 1];
          
          for (uint32_t idx = start; idx < end; ++idx) {
            const uint32_t u = edge_ptr[idx];
            if (lev_ptr[u] == depth) {
              lev_ptr[v] = next_depth;
              vis_ptr[v >> 6] |= (1ULL << (v & 63));
              next_frontier.push_back(v);
              break;
            }
          }
        }
        frontier_edges = 0;
        for (uint32_t v : next_frontier) {
          frontier_edges += off_ptr[v + 1] - off_ptr[v];
        }
      } else {
        // Top-down: expand from frontier
        frontier_edges = 0;
        for (uint32_t u : frontier) {
          const uint32_t start = off_ptr[u];
          const uint32_t end = off_ptr[u + 1];
          
          for (uint32_t idx = start; idx < end; ++idx) {
            const uint32_t v = edge_ptr[idx];
            const uint64_t bit = 1ULL << (v & 63);
            const uint32_t word = v >> 6;
            
            if (!(vis_ptr[word] & bit)) {
              vis_ptr[word] |= bit;
              lev_ptr[v] = next_depth;
              next_frontier.push_back(v);
              frontier_edges += off_ptr[v + 1] - off_ptr[v];
            }
          }
        }
      }
      
      frontier.swap(next_frontier);
      ++depth;
    }
    
    hash = checksum_levels(lev_ptr, n);
  }
  
  return hash;
}