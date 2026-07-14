#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_levels(const int32_t* levels, uint32_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  uint32_t i = 0;
  for (; i + 4 <= n; i += 4) {
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(levels[i] + 1));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(levels[i+1] + 1));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(levels[i+2] + 1));
    hash *= prime;
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(levels[i+3] + 1));
    hash *= prime;
  }
  for (; i < n; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(levels[i] + 1));
    hash *= prime;
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
  
  std::vector<int32_t> levels(n);
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next;
  frontier.reserve(n);
  next.reserve(n);
  
  const uint32_t* off_ptr = offsets.data();
  const uint32_t* edge_ptr = edges.data();
  int32_t* lev_ptr = levels.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(lev_ptr, 0xFF, n * sizeof(int32_t)); // -1 in two's complement
    
    frontier.clear();
    frontier.push_back(source);
    lev_ptr[source] = 0;
    int32_t depth = 0;
    
    while (!frontier.empty()) {
      next.clear();
      const int32_t next_depth = depth + 1;
      
      for (uint32_t u : frontier) {
        const uint32_t start = off_ptr[u];
        const uint32_t end = off_ptr[u + 1];
        for (uint32_t idx = start; idx < end; ++idx) {
          const uint32_t v = edge_ptr[idx];
          if (lev_ptr[v] == -1) {
            lev_ptr[v] = next_depth;
            next.push_back(v);
          }
        }
      }
      
      frontier.swap(next);
      ++depth;
    }
    
    hash = checksum_levels(lev_ptr, n);
  }
  
  return hash;
}