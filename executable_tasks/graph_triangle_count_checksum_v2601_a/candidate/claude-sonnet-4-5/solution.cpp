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
        
        // Intersect neighbors of u (after v) with neighbors of v
        uint32_t u_idx = i + 1u;
        uint32_t v_idx = v_start;
        
        while (u_idx < u_end && v_idx < v_end) {
          const uint32_t w_u = edges[u_idx];
          const uint32_t w_v = edges[v_idx];
          
          if (w_u == w_v) {
            if (w_u > v) {
              ++count;
            }
            ++u_idx;
            ++v_idx;
          } else if (w_u < w_v) {
            ++u_idx;
          } else {
            ++v_idx;
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