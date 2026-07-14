#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t checksum_levels(const std::vector<std::pair<uint32_t, uint32_t>>& levels, uint32_t current_gen) {
  uint64_t hash = 1469598103934665603ULL;
  for (const auto& entry : levels) {
    int32_t level = (entry.first == current_gen) ? static_cast<int32_t>(entry.second) : -1;
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
  std::vector<std::pair<uint32_t, uint32_t>> levels(n);
  std::vector<uint32_t> bfs_queue;
  bfs_queue.reserve(n);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    const uint32_t current_gen = static_cast<uint32_t>(iter + 1);
    bfs_queue.clear();
    
    levels[source].first = current_gen;
    levels[source].second = 0;
    bfs_queue.push_back(source);

    size_t q_ptr = 0;
    while (q_ptr < bfs_queue.size()) {
      const uint32_t u = bfs_queue[q_ptr++];
      const uint32_t start = offsets[u];
      const uint32_t end = offsets[u + 1];
      for (uint32_t idx = start; idx < end; ++idx) {
        const uint32_t v = edges[idx];
        if (levels[v].first != current_gen) {
          levels[v].first = current_gen;
          levels[v].second = levels[u].second + 1;
          bfs_queue.push_back(v);
        }
      }
    }

    final_hash = checksum_levels(levels, current_gen);
  }
  return final_hash;
}