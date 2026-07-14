#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

// Compute FNV-1 64-bit checksum over levels, treating unstamped nodes as -1
static inline uint64_t checksum_levels_token(const int32_t* levels,
                                             const uint32_t* stamp,
                                             uint32_t n,
                                             uint32_t token) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t i = 0; i < n; ++i) {
    int32_t lvl = (stamp[i] == token) ? levels[i] : -1;
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(lvl + 1));
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
  if (n == 0) {
    // Empty graph: checksum over empty level array is FNV-1 of empty input
    // which is the initial value.
    return 1469598103934665603ULL;
  }

  const uint32_t* __restrict off = offsets.data();
  const uint32_t* __restrict nbr = edges.data();

  std::vector<int32_t> levels(n);               // values valid only when stamped
  std::vector<uint32_t> stamp(n, 0u);           // visitation stamp per iteration
  std::vector<uint32_t> q;                      // BFS queue
  q.reserve(n);

  uint64_t hash = 0;
  for (uint32_t iter = 0; iter < static_cast<uint32_t>(iters); ++iter) {
    const uint32_t token = iter + 1u;

    q.clear();
    q.push_back(source);
    stamp[source] = token;
    levels[source] = 0;

    size_t head = 0;
    while (head < q.size()) {
      const uint32_t u = q[head++];
      const int32_t next_level = levels[u] + 1;
      const uint32_t beg = off[u];
      const uint32_t end = off[u + 1u];
      for (uint32_t idx = beg; idx < end; ++idx) {
        const uint32_t v = nbr[idx];
        if (stamp[v] != token) {
          stamp[v] = token;
          levels[v] = next_level;
          q.push_back(v);
        }
      }
    }

    hash = checksum_levels_token(levels.data(), stamp.data(), n, token);
  }

  return hash;
}
