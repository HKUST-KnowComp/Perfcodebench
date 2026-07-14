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
        
        const uint32_t v_start = offsets[v];
        const uint32_t v_end = offsets[v + 1u];
        
        // For each w in N(u) where w > v, check if w is in N(v)
        for (uint32_t p = i + 1u; p < u_end; ++p) {
          const uint32_t w = edges[p];
          if (w <= v) continue;
          
          // Binary search for w in N(v)
          if (std::binary_search(edges.begin() + v_start, edges.begin() + v_end, w)) {
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