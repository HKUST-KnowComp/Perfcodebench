#include "interface.h"

#include <cstdint>
#include <set>
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
        std::set<uint32_t> nbrs(
            edges.begin() + static_cast<std::ptrdiff_t>(offsets[v]),
            edges.begin() + static_cast<std::ptrdiff_t>(offsets[v + 1u]));
        for (uint32_t p = i + 1u; p < offsets[u + 1u]; ++p) {
          const uint32_t w = edges[p];
          if (w <= v) continue;
          if (nbrs.find(w) != nbrs.end()) {
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
