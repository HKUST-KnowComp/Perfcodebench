#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t checksum_levels_raw(const int32_t* levels, uint32_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t i = 0; i < n; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(levels[i] + 1));
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

  std::vector<int32_t> levels(n, -1);
  std::vector<uint32_t> queue(n);
  std::vector<uint32_t> touched;
  touched.reserve(n);

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (uint32_t v : touched) levels[v] = -1;
    touched.clear();

    uint32_t head = 0;
    uint32_t tail = 0;

    levels[source] = 0;
    touched.push_back(source);
    queue[tail++] = source;

    while (head < tail) {
      const uint32_t u = queue[head++];
      const int32_t next_level = levels[u] + 1;

      const uint32_t begin = offsets[u];
      const uint32_t end = offsets[u + 1u];
      for (uint32_t idx = begin; idx < end; ++idx) {
        const uint32_t v = edges[idx];
        if (levels[v] == -1) {
          levels[v] = next_level;
          touched.push_back(v);
          queue[tail++] = v;
        }
      }
    }

    hash = checksum_levels_raw(levels.data(), n);
  }

  return hash;
}
