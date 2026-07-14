#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

namespace {

uint64_t checksum_levels(const int32_t* levels, uint32_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t i = 0; i < n; ++i) {
    uint32_t v = static_cast<uint32_t>(levels[i] + 1);
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t bfs_level_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    uint32_t source,
    int iters) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1);
  std::vector<int32_t> levels(n);
  std::vector<uint64_t> frontier((n + 63) / 64);
  std::vector<uint64_t> next_frontier((n + 63) / 64);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // reset level array
    std::memset(levels.data(), 0xFF, n * sizeof(int32_t)); // -1
    std::memset(frontier.data(), 0, frontier.size() * sizeof(uint64_t));
    std::memset(next_frontier.data(), 0, next_frontier.size() * sizeof(uint64_t));

    levels[source] = 0;
    frontier[source >> 6] |= 1ULL << (source & 63);
    int32_t depth = 0;

    while (true) {
      bool any = false;
      const uint64_t* fptr = frontier.data();
      uint64_t*       nptr = next_frontier.data();
      // scan entire bitmask
      for (uint32_t base = 0; base < n; base += 64) {
        uint64_t block = *fptr++;
        if (block == 0) { continue; }
        any = true;
        while (block) {
          uint32_t tz = __builtin_ctzll(block);
          uint32_t u  = base + tz;
          block &= block - 1;
          const uint32_t beg = offsets[u];
          const uint32_t end = offsets[u + 1];
          for (uint32_t idx = beg; idx < end; ++idx) {
            uint32_t v = edges[idx];
            int32_t  lv = levels[v];
            if (lv == -1) {
              levels[v] = depth + 1;
              nptr[v >> 6] |= 1ULL << (v & 63);
            }
          }
        }
      }
      if (!any) break;
      ++depth;
      frontier.swap(next_frontier);
      std::memset(next_frontier.data(), 0, next_frontier.size() * sizeof(uint64_t));
    }
    hash = checksum_levels(levels.data(), n);
  }
  return hash;
}