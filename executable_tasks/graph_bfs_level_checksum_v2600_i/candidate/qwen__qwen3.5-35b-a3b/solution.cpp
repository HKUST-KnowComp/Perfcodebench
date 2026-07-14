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
  frontier.reserve(n);
  next.reserve(n);
  
  uint64_t hash = 0;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(levels.begin(), levels.end(), -1);
    frontier.clear();
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
    
    uint64_t current_hash = FNV_OFFSET;
    for (int32_t level : levels) {
      current_hash ^= static_cast<uint64_t>(static_cast<uint32_t>(level + 1));
      current_hash *= FNV_PRIME;
    }
    hash = current_hash;
  }
  return hash;
}