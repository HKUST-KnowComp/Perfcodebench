#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>
#include <cstring>

namespace {

uint64_t checksum_levels(const std::vector<int32_t>& levels) {
  uint64_t hash = 1469598103934665603ULL;
  for (int32_t level : levels) {
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
  if (n == 0) return 0;

  std::vector<int32_t> levels(n);
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next_frontier;
  frontier.reserve(n);
  next_frontier.reserve(n);

  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    frontier.clear();
    
    levels[source] = 0;
    frontier.push_back(source);
    
    int32_t depth = 0;
    while (!frontier.empty()) {
      next_frontier.clear();
      for (uint32_t u : frontier) {
        const uint32_t start = offsets[u];
        const uint32_t end = offsets[u + 1];
        for (uint32_t i = start; i < end; ++i) {
          uint32_t v = edges[i];
          if (levels[v] == -1) {
            levels[v] = depth + 1;
            next_frontier.push_back(v);
          }
        }
      }
      
      // To handle duplicates in next_frontier (multiple parents reaching same node in same level)
      // we must ensure we don't add the same node to next_frontier multiple times.
      // However, the 'levels[v] == -1' check already prevents re-visiting nodes from previous levels.
      // To prevent duplicates within the SAME level, we need a way to mark them as 'seen'.
      // A simple way is to set levels[v] immediately when discovered.
      // The current logic 'levels[v] = depth + 1' inside the loop already handles this correctly
      // because the next time the same 'v' is encountered in the same frontier expansion,
      // levels[v] will no longer be -1.
      
      frontier.swap(next_frontier);
      depth++;
    }
    final_hash = checksum_levels(levels);
  }

  return final_hash;
}