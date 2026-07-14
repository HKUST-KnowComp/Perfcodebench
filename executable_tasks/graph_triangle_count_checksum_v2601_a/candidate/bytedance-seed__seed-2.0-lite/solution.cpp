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
      const uint32_t u_end = offsets[u + 1u];
      for (uint32_t i = offsets[u]; i < u_end; ++i) {
        const uint32_t v = edges[i];
        if (v <= u) continue;
        // Two-pointer intersection of sorted adjacency lists to count common neighbors
        size_t a = offsets[v];
        const size_t a_end = offsets[v + 1u];
        size_t b = i + 1u;
        while (a < a_end && b < u_end) {
          const uint32_t ea = edges[a];
          const uint32_t eb = edges[b];
          if (ea == eb) {
            ++count;
            ++a;
            ++b;
          } else if (ea < eb) {
            ++a;
          } else {
            ++b;
          }
        }
      }
    }
    // Exact original hash calculation to preserve checksum correctness
    hash = 1469598103934665603ULL;
    hash ^= count;
    hash *= 1099511628211ULL;
  }
  return hash;
}