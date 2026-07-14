#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t triangle_count_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    int iters) {
  uint64_t hash = 0;
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  const uint32_t* off_ptr = offsets.data();
  const uint32_t* edge_ptr = edges.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    for (uint32_t u = 0; u < n; ++u) {
      uint32_t u_start = off_ptr[u];
      uint32_t u_end = off_ptr[u + 1];
      for (uint32_t i = u_start; i < u_end; ++i) {
        uint32_t v = edge_ptr[i];
        if (v <= u) continue;
        
        uint32_t v_start = off_ptr[v];
        uint32_t v_end = off_ptr[v + 1];
        
        const uint32_t* p1 = edge_ptr + i + 1;
        const uint32_t* p1_end = edge_ptr + u_end;
        const uint32_t* p2 = edge_ptr + v_start;
        const uint32_t* p2_end = edge_ptr + v_end;
        
        while (p1 < p1_end && p2 < p2_end) {
          uint32_t w1 = *p1;
          uint32_t w2 = *p2;
          if (w1 == w2) {
            ++count;
            ++p1;
            ++p2;
          } else if (w1 < w2) {
            ++p1;
          } else {
            ++p2;
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