#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

uint64_t bfs_level_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    uint32_t source,
    int iters) {
  if (offsets.empty()) return 0;
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  
  std::vector<int32_t> levels(n, -1);
  std::vector<uint32_t> q1(n);
  std::vector<uint32_t> q2(n);
  
  const uint32_t* off_data = offsets.data();
  const uint32_t* edge_data = edges.data();
  int32_t* levels_data = levels.data();
  uint32_t* frontier = q1.data();
  uint32_t* next_frontier = q2.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels_data, levels_data + n, -1);
    
    uint32_t f_size = 1;
    frontier[0] = source;
    levels_data[source] = 0;
    int32_t depth = 0;

    while (f_size > 0) {
      uint32_t nf_size = 0;
      int32_t next_depth = depth + 1;
      for (uint32_t i = 0; i < f_size; ++i) {
        uint32_t u = frontier[i];
        uint32_t start = off_data[u];
        uint32_t end = off_data[u + 1];
        for (uint32_t idx = start; idx < end; ++idx) {
          uint32_t v = edge_data[idx];
          if (levels_data[v] == -1) {
            levels_data[v] = next_depth;
            next_frontier[nf_size++] = v;
          }
        }
      }
      std::swap(frontier, next_frontier);
      f_size = nf_size;
      depth = next_depth;
    }

    uint64_t h = 1469598103934665603ULL;
    for (uint32_t i = 0; i < n; ++i) {
      h ^= static_cast<uint64_t>(static_cast<uint32_t>(levels_data[i] + 1));
      h *= 1099511628211ULL;
    }
    hash = h;
  }
  return hash;
}
