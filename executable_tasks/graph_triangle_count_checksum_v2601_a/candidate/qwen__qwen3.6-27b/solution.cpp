#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t triangle_count_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    int iters) {
  uint64_t hash = 0;
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  const uint32_t* off = offsets.data();
  const uint32_t* edg = edges.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    for (uint32_t u = 0; u < n; ++u) {
      const uint32_t* u_start = edg + off[u];
      const uint32_t* u_end = edg + off[u + 1];

      const uint32_t* it_v = u_start;
      while (it_v < u_end && *it_v <= u) ++it_v;

      for (; it_v < u_end; ++it_v) {
        uint32_t v = *it_v;
        const uint32_t* v_start = edg + off[v];
        const uint32_t* v_end = edg + off[v + 1];

        const uint32_t* p1 = it_v + 1;
        const uint32_t* p2 = v_start;

        while (p2 < v_end && *p2 <= u) ++p2;

        while (p1 < u_end && p2 < v_end) {
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