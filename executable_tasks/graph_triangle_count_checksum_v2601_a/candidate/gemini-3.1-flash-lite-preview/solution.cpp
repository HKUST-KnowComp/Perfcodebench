#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t triangle_count_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    int iters) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  uint64_t count = 0;

  for (uint32_t u = 0; u < n; ++u) {
    uint32_t start_u = offsets[u];
    uint32_t end_u = offsets[u + 1];
    for (uint32_t i = start_u; i < end_u; ++i) {
      uint32_t v = edges[i];
      if (v <= u) continue;

      uint32_t start_v = offsets[v];
      uint32_t end_v = offsets[v + 1];

      uint32_t p1 = i + 1;
      uint32_t p2 = start_v;

      while (p1 < end_u && p2 < end_v) {
        uint32_t w1 = edges[p1];
        uint32_t w2 = edges[p2];
        if (w1 == w2) {
          count++;
          p1++;
          p2++;
        } else if (w1 < w2) {
          p1++;
        } else {
          p2++;
        }
      }
    }
  }

  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    hash ^= count;
    hash *= 1099511628211ULL;
  }
  return hash;
}