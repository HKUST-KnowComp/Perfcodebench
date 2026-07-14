#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

inline uint64_t compute_checksum(const int32_t* levels, uint32_t n) {
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
  if (n == 0) return 0;

  std::vector<int32_t> levels(n);
  std::vector<uint32_t> current_frontier;
  std::vector<uint32_t> next_frontier;
  current_frontier.reserve(n);
  next_frontier.reserve(n);

  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    current_frontier.clear();
    
    levels[source] = 0;
    current_frontier.push_back(source);
    
    int32_t depth = 0;
    while (!current_frontier.empty()) {
      next_frontier.clear();
      for (uint32_t u : current_frontier) {
        const uint32_t start = offsets[u];
        const uint32_t end = offsets[u + 1];
        for (uint32_t i = start; i < end; ++i) {
          const uint32_t v = edges[i];
          if (levels[v] == -1) {
            levels[v] = depth + 1;
            next_frontier.push_back(v);
          }
        }
      }
      current_frontier.swap(next_frontier);
      ++depth;
    }
    final_hash = compute_checksum(levels.data(), n);
  }
  
  return final_hash;
}