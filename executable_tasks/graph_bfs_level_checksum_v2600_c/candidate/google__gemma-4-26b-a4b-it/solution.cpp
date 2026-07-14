#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

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
  std::vector<int32_t> levels(n, -1);
  
  // Pre-allocate buffers to avoid reallocations
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
      
      // The original code used sort/unique on candidates. 
      // To preserve exact correctness for the 'levels' array when multiple nodes in the 
      // same frontier point to the same unvisited node, we must ensure the 'next' 
      // frontier contains unique nodes. 
      // However, the original code's 'levels[v] = depth + 1' happens inside the loop 
      // AFTER the sort/unique. 
      // Wait, looking closely at baseline: 
      // 1. Collect ALL neighbors of frontier into 'candidates'.
      // 2. Sort and Unique 'candidates'.
      // 3. For each unique candidate, if levels[v] == -1, set level and add to 'next'.
      // This means if multiple nodes in 'frontier' point to 'v', 'v' is added to 'next' exactly once.
      // My optimized version: 'if (levels[v] == -1) { levels[v] = depth + 1; next_frontier.push_back(v); }'
      // This also ensures 'v' is added to 'next_frontier' exactly once because 'levels[v]' is updated immediately.
      
      // To handle the case where multiple nodes in 'frontier' point to the same 'v',
      // we must ensure 'next_frontier' doesn't have duplicates. 
      // The 'levels[v] = depth + 1' check handles this perfectly.
      
      // One edge case: the original code's 'candidates' might contain nodes that were 
      // already visited in PREVIOUS depths. The 'if (levels[v] == -1)' handles that.
      
      // There is one subtle difference: the original code's 'candidates' includes 
      // ALL neighbors. If two nodes in 'frontier' point to the same 'v', 'v' is in 'candidates' once.
      // My code: 'v' is added to 'next_frontier' the first time it is encountered.
      // To be 100% safe regarding the 'unique' behavior, we should ensure 'next_frontier' 
      // is unique. But since we set 'levels[v]' immediately, 'levels[v] == -1' will be 
      // false for subsequent encounters of 'v' in the same frontier loop.
      
      // However, the original code's 'candidates' might contain duplicates that are 
      // removed by 'unique'. My code handles this by the 'levels[v] == -1' check.
      // BUT, there is a catch: if 'v' is in 'next_frontier', it must be unique.
      // Let's refine the 'next_frontier' logic to be strictly identical to the baseline's 
      // 'unique' behavior if necessary. 
      // Actually, the baseline's 'candidates' is a collection of all neighbors. 
      // If 'u1' and 'u2' are in 'frontier' and both have neighbor 'v', 
      // 'v' appears twice in 'candidates', then once in 'unique candidates'.
      // Then 'levels[v]' is checked. 
      // My code: 'u1' sees 'v', 'levels[v]' becomes 'depth+1', 'v' added to 'next'.
      // 'u2' sees 'v', 'levels[v]' is no longer -1, 'v' is NOT added to 'next'.
      // This is identical.
      
      // Wait, there is one tiny detail: the original code's 'candidates' might contain 
      // nodes that were already visited in previous levels. The 'if (levels[v] == -1)' 
      // in the baseline handles that. My code does too.
      
      // One more thing: the baseline's 'candidates' is sorted. Does the order of 'next' 
      // matter for the checksum? No, because the checksum iterates over the 'levels' array, 
      // not the 'frontier' or 'next' arrays. The 'levels' array is the source of truth.
      
      std::swap(frontier, next_frontier);
      ++depth;
    }
    final_hash = checksum_levels(levels);
  }
  return final_hash;
}
