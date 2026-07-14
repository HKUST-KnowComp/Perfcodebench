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
  std::vector<uint32_t> queue(n);
  std::vector<uint8_t> visited(n);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    std::memset(visited.data(), 0, n);
    
    uint32_t read_pos = 0;
    uint32_t write_pos = 0;
    
    queue[write_pos++] = source;
    levels[source] = 0;
    visited[source] = 1;
    
    int32_t current_level = 0;
    uint32_t level_end = write_pos;
    
    while (read_pos < write_pos) {
      uint32_t next_level_end = write_pos;
      
      while (read_pos < level_end) {
        uint32_t u = queue[read_pos++];
        
        for (uint32_t idx = offsets[u]; idx < offsets[u + 1u]; ++idx) {
          uint32_t v = edges[idx];
          if (!visited[v]) {
            visited[v] = 1;
            levels[v] = current_level + 1;
            queue[write_pos++] = v;
          }
        }
      }
      
      level_end = next_level_end;
      ++current_level;
    }
    
    hash = checksum_levels(levels);
  }
  return hash;
}
