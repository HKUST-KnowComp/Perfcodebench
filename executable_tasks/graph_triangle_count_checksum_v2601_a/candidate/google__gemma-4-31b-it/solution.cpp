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
      const uint32_t u_end = offsets[u + 1];

      for (uint32_t i = u_start; i < u_end; ++i) {
        const uint32_t v = edges[i];
        if (v <= u) continue;

        const uint32_t v_start = offsets[v];
        const uint32_t v_end = offsets[v + 1];

        // We need to count w such that w > v and w is a neighbor of both u and v.
        // Since edges are sorted, we can use a two-pointer approach to find the intersection
        // of the adjacency lists of u and v, starting from the first element > v.
        
        uint32_t u_ptr = i + 1;
        uint32_t v_ptr = v_start;

        // Skip elements in v's list that are <= v
        // (Though usually v's list is sorted, we only care about w > v)
        while (v_ptr < v_end && edges[v_ptr] <= v) {
          v_ptr++;
        }

        while (u_ptr < u_end && v_ptr < v_end) {
          const uint32_t u_val = edges[u_ptr];
          const uint32_t v_val = edges[v_ptr];

          if (u_val == v_val) {
            count++;
            u_ptr++;
            v_ptr++;
          } else if (u_val < v_val) {
            u_ptr++;
          } else {
            v_ptr++;
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