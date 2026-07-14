#include "interface.h"
#include <vector>
#include <algorithm>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
    std::fill(best.begin(), best.end(), 0u);
    
    const size_t n = groups.size();
    const uint32_t* g_ptr = groups.data();
    const uint32_t* v_ptr = values.data();
    uint32_t* b_ptr = best.data();

    for (size_t i = 0; i < n; ++i) {
      uint32_t g = g_ptr[i];
      uint32_t v = v_ptr[i];
      if (v > b_ptr[g]) {
        b_ptr[g] = v;
      }
    }

    hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, b_ptr[g]);
    }
  }
  return hash;
}