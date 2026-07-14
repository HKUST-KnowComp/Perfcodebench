#include "interface.h"

#include <vector>
#include <cstring>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  std::vector<uint32_t> best(group_count);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(best.data(), 0, group_count * sizeof(uint32_t));
    
    const std::size_t size = groups.size();
    for (std::size_t i = 0; i < size; ++i) {
      if (i + 8 < size) {
        __builtin_prefetch(&best[groups[i + 8]], 1, 1);
      }
      uint32_t g = groups[i];
      uint32_t v = values[i];
      if (v > best[g]) {
        best[g] = v;
      }
    }
    
    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, best[g]);
    }
  }
  return hash;
}