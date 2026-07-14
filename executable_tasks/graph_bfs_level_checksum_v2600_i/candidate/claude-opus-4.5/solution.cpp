#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

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
  
  const uint32_t* __restrict off_ptr = offsets.data();
  const uint32_t* __restrict edge_ptr = edges.data();
  
  std::vector<int32_t> levels(n);
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next;
  frontier.reserve(n);
  next.reserve(n);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(levels.data(), 0xFF, n * sizeof(int32_t));
    
    frontier.clear();
    frontier.push_back(source);
    levels[source] = 0;
    int32_t depth = 0;
    
    uint64_t frontier_edges = off_ptr[source + 1] - off_ptr[source];
    const uint64_t total_edges = edges.size();
    const uint32_t alpha = 15;
    const uint32_t beta = 24;
    
    bool bottom_up_mode = false;
    
    while (!frontier.empty()) {
      next.clear();
      int32_t next_depth = depth + 1;
      
      if (!bottom_up_mode && frontier_edges > total_edges / alpha) {
        bottom_up_mode = true;
      }
      
      if (bottom_up_mode && frontier.size() < n / beta) {
        bottom_up_mode = false;
      }
      
      if (bottom_up_mode) {
        // Bottom-up BFS: iterate over all unvisited nodes
        for (uint32_t v = 0; v < n; ++v) {
          if (levels[v] == -1) {
            uint32_t start = off_ptr[v];
            uint32_t end = off_ptr[v + 1];
            for (uint32_t idx = start; idx < end; ++idx) {
              uint32_t u = edge_ptr[idx];
              if (levels[u] == depth) {
                levels[v] = next_depth;
                next.push_back(v);
                break;
              }
            }
          }
        }
      } else {
        // Top-down BFS: expand from frontier
        for (uint32_t u : frontier) {
          uint32_t start = off_ptr[u];
          uint32_t end = off_ptr[u + 1];
          for (uint32_t idx = start; idx < end; ++idx) {
            uint32_t v = edge_ptr[idx];
            if (levels[v] == -1) {
              levels[v] = next_depth;
              next.push_back(v);
            }
          }
        }
      }
      
      // Compute frontier edges for next iteration
      frontier_edges = 0;
      for (uint32_t v : next) {
        frontier_edges += off_ptr[v + 1] - off_ptr[v];
      }
      
      frontier.swap(next);
      ++depth;
    }
    
    hash = checksum_levels(levels.data(), n);
  }
  
  return hash;
}