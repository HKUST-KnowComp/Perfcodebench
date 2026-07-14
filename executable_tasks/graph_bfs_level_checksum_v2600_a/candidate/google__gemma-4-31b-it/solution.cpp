#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

namespace {

inline uint64_t compute_checksum(const int32_t* levels, uint32_t n) {
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
  if (n == 0) return 0;

  std::vector<int32_t> levels(n);
  std::vector<uint32_t> queue(n);
  
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    
    uint32_t head = 0;
    uint32_t tail = 0;
    
    levels[source] = 0;
    queue[tail++] = source;

    while (head < tail) {
      uint32_t u = queue[head++];
      int32_t next_level = levels[u] + 1;
      
      const uint32_t start = offsets[u];
      const uint32_t end = offsets[u + 1];
      
      for (uint32_t i = start; i < end; ++i) {
        uint32_t v = edges[i];
        if (levels[v] == -1) {
          levels[v] = next_level;
          queue[tail++] = v;
        }
      }
    }
    final_hash = compute_checksum(levels.data(), n);
  }

  return final_hash;
}