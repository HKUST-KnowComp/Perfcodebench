#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t triangle_count_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    int iters) {
  uint64_t hash = 0;
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    
    for (uint32_t u = 0; u < n; ++u) {
      const uint32_t u_start = offsets[u];
      const uint32_t u_end = offsets[u + 1u];
      
      for (uint32_t i = u_start; i < u_end; ++i) {
        const uint32_t v = edges[i];
        if (v <= u) continue;
        
        for (uint32_t j = i + 1; j < u_end; ++j) {
          const uint32_t w = edges[j];
          if (w <= v) continue;
          
          const uint32_t v_start = offsets[v];
          const uint32_t v_end = offsets[v + 1u];
          
          const uint32_t* v_neighbors = edges.data() + v_start;
          if (std::binary_search(v_neighbors, v_neighbors + (v_end - v_start), w)) {
            ++count;
          }
        }
      }
    }
    
    hash = 1469598103934665603ULL;
    hash ^= count;
    hash *= 1099511628211ULL;
  }
  
  return hash;
}