#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t triangle_count_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    int iters) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);

  // Precompute the first index in each adjacency list whose neighbor is > u.
  // Since CSR rows are sorted, this lets us skip all backward/low-id neighbors.
  std::vector<uint32_t> forward_begin(n);
  for (uint32_t u = 0; u < n; ++u) {
    uint32_t lo = offsets[u];
    uint32_t hi = offsets[u + 1u];
    while (lo < hi) {
      const uint32_t mid = lo + ((hi - lo) >> 1);
      if (edges[mid] <= u) {
        lo = mid + 1u;
      } else {
        hi = mid;
      }
    }
    forward_begin[u] = lo;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;

    for (uint32_t u = 0; u < n; ++u) {
      const uint32_t u_end = offsets[u + 1u];
      for (uint32_t i = forward_begin[u]; i < u_end; ++i) {
        const uint32_t v = edges[i];

        // Count intersection of:
        //   neighbors of u after position i   => w > v in sorted row of u
        //   neighbors of v with id > v        => w > v in sorted row of v
        uint32_t a = i + 1u;
        uint32_t b = forward_begin[v];
        const uint32_t b_end = offsets[v + 1u];

        while (a < u_end && b < b_end) {
          const uint32_t x = edges[a];
          const uint32_t y = edges[b];
          if (x < y) {
            ++a;
          } else if (y < x) {
            ++b;
          } else {
            ++count;
            ++a;
            ++b;
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
