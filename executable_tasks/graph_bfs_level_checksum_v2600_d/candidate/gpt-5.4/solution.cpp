#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t checksum_levels_stamped(const std::vector<int32_t>& levels,
                                 const std::vector<uint32_t>& seen_stamp,
                                 uint32_t cur_stamp) {
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t n = static_cast<uint32_t>(levels.size());
  for (uint32_t i = 0; i < n; ++i) {
    const int32_t level = (seen_stamp[i] == cur_stamp) ? levels[i] : -1;
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(level + 1));
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
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  if (n == 0 || iters <= 0) return 0;

  std::vector<int32_t> levels(n, 0);
  std::vector<uint32_t> seen_stamp(n, 0);
  std::vector<uint32_t> queue(n);

  uint32_t cur_stamp = 1;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    if (source >= n) {
      uint64_t h = 1469598103934665603ULL;
      for (uint32_t i = 0; i < n; ++i) {
        h ^= 0ULL;
        h *= 1099511628211ULL;
      }
      hash = h;
      continue;
    }

    if (cur_stamp == 0) {
      for (uint32_t i = 0; i < n; ++i) seen_stamp[i] = 0;
      cur_stamp = 1;
    }

    uint32_t head = 0;
    uint32_t tail = 0;
    queue[tail++] = source;
    seen_stamp[source] = cur_stamp;
    levels[source] = 0;

    while (head < tail) {
      const uint32_t u = queue[head++];
      const int32_t next_level = levels[u] + 1;
      const uint32_t begin = offsets[u];
      const uint32_t end = offsets[u + 1u];
      for (uint32_t idx = begin; idx < end; ++idx) {
        const uint32_t v = edges[idx];
        if (seen_stamp[v] != cur_stamp) {
          seen_stamp[v] = cur_stamp;
          levels[v] = next_level;
          queue[tail++] = v;
        }
      }
    }

    hash = checksum_levels_stamped(levels, seen_stamp, cur_stamp);
    ++cur_stamp;
  }

  return hash;
}
