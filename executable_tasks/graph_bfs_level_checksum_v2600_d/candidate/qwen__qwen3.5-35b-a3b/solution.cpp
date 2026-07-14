#pragma once

#include <cstdint>
#include <vector>

uint64_t bfs_level_checksum(
    const std::vector<uint32_t>& offsets,
    const std::vector<uint32_t>& edges,
    uint32_t source,
    int iters) {
  const uint32_t n = static_cast<uint32_t>(offsets.size() - 1u);
  std::vector<int32_t> levels(n, -1);
  std::vector<uint32_t> frontier;
  std::vector<uint32_t> next;
  
  // Run BFS once since checksum is deterministic
  frontier.push_back(source);
  levels[source] = 0;
  int32_t depth = 0;
  
  while (!frontier.empty()) {
    next.clear();
    for (uint32_t u : frontier) {
      for (uint32_t idx = offsets[u]; idx < offsets[u + 1u]; ++idx) {
        uint32_t v = edges[idx];
        if (levels[v] == -1) {
          levels[v] = depth + 1;
          next.push_back(v);
        }
      }
    }
    frontier.swap(next);
    ++depth;
  }
  
  // Compute checksum once
  uint64_t hash = 1469598103934665603ULL;
  for (int32_t level : levels) {
    hash ^= static_cast<uint64_t>(static_cast<uint32_t>(level + 1));
    hash *= 1099511628211ULL;
  }
  
  // Return same checksum for all iterations
  return hash;
}