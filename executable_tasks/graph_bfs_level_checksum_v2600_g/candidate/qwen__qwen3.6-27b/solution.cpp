#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>
#include <cstring>

uint64_t bfs_level_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    uint32_t source,
    int iters) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  std::vector<int32_t> levels(n, -1);
  std::vector<uint32_t> curr;
  std::vector<uint32_t> next;
  curr.reserve(n);
  next.reserve(n);

  const uint32_t* off = offsets.data();
  const uint32_t* edg = edges.data();
  int32_t* lev = levels.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(lev, -1, n * sizeof(int32_t));
    curr.clear();
    next.clear();
    curr.push_back(source);
    lev[source] = 0;
    int32_t depth = 0;

    while (!curr.empty()) {
      ++depth;
      for (uint32_t u : curr) {
        for (uint32_t idx = off[u]; idx < off[u + 1]; ++idx) {
          uint32_t v = edg[idx];
          if (lev[v] == -1) {
            lev[v] = depth;
            next.push_back(v);
          }
        }
      }
      curr.swap(next);
      next.clear();
    }

    hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < n; ++i) {
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(lev[i] + 1));
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}