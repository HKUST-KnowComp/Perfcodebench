#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t triangle_count_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    int iters) {
  uint64_t hash = 0;
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    for (uint32_t u = 0; u < n; ++u) {
      for (uint32_t i = offsets[u]; i < offsets[u + 1u]; ++i) {
        const uint32_t v = edges[i];
        if (v <= u) continue;
        uint32_t p = i + 1u;
        uint32_t q = offsets[v];
        while (p < offsets[u + 1u] && q < offsets[v + 1u]) {
          const uint32_t a = edges[p];
          const uint32_t b = edges[q];
          if (a <= v) {
            ++p;
          } else if (b <= v) {
            ++q;
          } else if (a == b) {
            ++count;
            ++p;
            ++q;
          } else if (a < b) {
            ++p;
          } else {
            ++q;
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
